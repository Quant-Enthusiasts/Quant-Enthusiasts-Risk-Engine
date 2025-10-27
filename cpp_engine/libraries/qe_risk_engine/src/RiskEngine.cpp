#include "RiskEngine.h"
#include <numeric>
#include <random>
#include <algorithm>
#include <vector>
#include <cmath>
#include <sstream>
#include <limits>

RiskEngine::RiskEngine() 
    : var_simulations_(10000),
      time_horizon_days_(1.0),
      random_seed_(0),
      use_fixed_seed_(false) {
}

RiskEngine::RiskEngine(int var_simulations)
    : var_simulations_(var_simulations),
      time_horizon_days_(1.0),
      random_seed_(0),
      use_fixed_seed_(false) {
    validateParameters();
}

void RiskEngine::setMarketDataCachePath(const std::string& db_path) {
    if (db_path.empty()) {
        local_market_db_.reset();  // Clear any existing DB
        market_data_manager_.setLocalDB(nullptr);
        market_data_cache_path_.clear();
        return;
    }

    market_data_cache_path_ = db_path;
    local_market_db_ = std::make_unique<LocalMarketDB>(db_path);
    
    if (!local_market_db_->init()) {
        throw std::runtime_error("Failed to initialize market data cache at: " + db_path);
    }
    
    market_data_manager_.setLocalDB(local_market_db_.get());
}

void RiskEngine::setVaRSimulations(int simulations) {
    if (simulations <= 0) {
        throw std::invalid_argument("VaR simulations must be positive");
    }
    if (simulations > 1000000) {
        throw std::invalid_argument("VaR simulations cannot exceed 1,000,000");
    }
    var_simulations_ = simulations;
}

int RiskEngine::getVaRSimulations() const {
    return var_simulations_;
}

void RiskEngine::setVaRTimeHorizonDays(double days) {
    if (days <= 0.0) {
        throw std::invalid_argument("Time horizon must be positive");
    }
    if (days > 252.0) {
        throw std::invalid_argument("Time horizon cannot exceed 252 trading days");
    }
    time_horizon_days_ = days;
}

double RiskEngine::getVaRTimeHorizonDays() const {
    return time_horizon_days_;
}

void RiskEngine::setRandomSeed(unsigned int seed) {
    random_seed_ = seed;
    use_fixed_seed_ = true;
}

void RiskEngine::setUseFixedSeed(bool use_fixed) {
    use_fixed_seed_ = use_fixed;
}

void RiskEngine::validateParameters() const {
    if (var_simulations_ <= 0 || var_simulations_ > 1000000) {
        throw std::invalid_argument("Invalid VaR simulations parameter");
    }
    if (time_horizon_days_ <= 0.0 || time_horizon_days_ > 252.0) {
        throw std::invalid_argument("Invalid time horizon parameter");
    }
}

void RiskEngine::validateMarketData(
    const Portfolio& portfolio,
    const std::map<std::string, MarketData>& market_data_map
) const {
    const auto& instruments = portfolio.getInstruments();
    
    for (const auto& [instrument, quantity] : instruments) {
        if (!instrument) {
            throw std::runtime_error("Portfolio contains null instrument");
        }
        
        std::string asset_id;
        try {
            asset_id = instrument->getAssetId();
        } catch (const std::exception& e) {
            throw std::runtime_error(std::string("Failed to get asset ID: ") + e.what());
        }
        
        if (asset_id.empty()) {
            throw std::runtime_error("Instrument has empty asset ID");
        }
        
        if (market_data_map.find(asset_id) == market_data_map.end()) {
            throw std::runtime_error("Missing market data for asset: " + asset_id);
        }
        
        const MarketData& md = market_data_map.at(asset_id);
        
        if (md.spot_price <= 0.0) {
            throw std::invalid_argument("Spot price must be positive for " + asset_id);
        }
        if (md.volatility < 0.0) {
            throw std::invalid_argument("Volatility cannot be negative for " + asset_id);
        }
        if (std::isnan(md.spot_price) || std::isinf(md.spot_price)) {
            throw std::invalid_argument("Invalid spot price for " + asset_id);
        }
        if (std::isnan(md.risk_free_rate) || std::isinf(md.risk_free_rate)) {
            throw std::invalid_argument("Invalid risk-free rate for " + asset_id);
        }
        if (std::isnan(md.volatility) || std::isinf(md.volatility)) {
            throw std::invalid_argument("Invalid volatility for " + asset_id);
        }
    }
}

double RiskEngine::calculateSingleInstrumentMetric(
    const std::unique_ptr<Instrument>& instrument,
    int quantity,
    const MarketData& md,
    const std::string& metric_name
) const {
    double metric_value = 0.0;
    
    try {
        if (metric_name == "price") {
            metric_value = instrument->price(md);
        } else if (metric_name == "delta") {
            metric_value = instrument->delta(md);
        } else if (metric_name == "gamma") {
            metric_value = instrument->gamma(md);
        } else if (metric_name == "vega") {
            metric_value = instrument->vega(md);
        } else if (metric_name == "theta") {
            metric_value = instrument->theta(md);
        }
    } catch (const std::exception& e) {
        throw std::runtime_error(
            std::string("Failed to calculate ") + metric_name + 
            " for " + instrument->getAssetId() + ": " + e.what()
        );
    }
    
    if (std::isnan(metric_value) || std::isinf(metric_value)) {
        throw std::runtime_error(
            "Invalid " + metric_name + " value for " + instrument->getAssetId()
        );
    }
    
    double result = metric_value * quantity;
    
    if (std::isnan(result) || std::isinf(result)) {
        throw std::overflow_error(
            "Overflow in " + metric_name + " calculation for " + instrument->getAssetId()
        );
    }
    
    return result;
}

PortfolioRiskResult RiskEngine::calculatePortfolioRisk(
    const Portfolio& portfolio, 
    const std::map<std::string, MarketData>& market_data_map
) {
    for (const auto& [asset_id, md] : market_data_map) {
        if (market_data_manager_.hasMarketData(asset_id)) {
            market_data_manager_.updateMarketData(asset_id, md);
        } else {
            market_data_manager_.addMarketData(asset_id, md);
        }
    }

    validateParameters();
    
    PortfolioRiskResult result;
    result.reset();
    
    if (portfolio.empty()) {
        return result;
    }
    
    validateMarketData(portfolio, market_data_manager_.getAllMarketData());
    
    const auto& instruments = portfolio.getInstruments();
    const auto& cached_market_data = market_data_manager_.getAllMarketData();
    
    for (const auto& [instrument, quantity] : instruments) {
        std::string asset_id = instrument->getAssetId();
        const MarketData& md = cached_market_data.at(asset_id);
        
        result.total_pv += calculateSingleInstrumentMetric(instrument, quantity, md, "price");
        result.total_delta += calculateSingleInstrumentMetric(instrument, quantity, md, "delta");
        result.total_gamma += calculateSingleInstrumentMetric(instrument, quantity, md, "gamma");
        result.total_vega += calculateSingleInstrumentMetric(instrument, quantity, md, "vega");
        result.total_theta += calculateSingleInstrumentMetric(instrument, quantity, md, "theta");
    }
    
    if (!result.isValid()) {
        throw std::runtime_error("Portfolio risk calculation produced invalid results");
    }
    
    try {
        RiskMetrics metrics = calculateRiskMetrics(portfolio, market_data_map);
        result.value_at_risk_95 = metrics.var_95;
        result.value_at_risk_99 = metrics.var_99;
        result.expected_shortfall_95 = metrics.es_95;
        result.expected_shortfall_99 = metrics.es_99;
    } catch (const std::exception& e) {
        throw std::runtime_error(std::string("Risk metrics calculation failed: ") + e.what());
    }
    
    return result;
}

RiskMetrics RiskEngine::calculateRiskMetrics(
    const Portfolio& portfolio, 
    const std::map<std::string, MarketData>& market_data_map
) {
    const auto& cached_market_data = market_data_manager_.getAllMarketData();
    RiskMetrics metrics;
    
    // Calculate initial portfolio value
    double initial_portfolio_value = 0.0;
    const auto& instruments = portfolio.getInstruments();
    
    for (const auto& [instrument, quantity] : instruments) {
        const MarketData& md = cached_market_data.at(instrument->getAssetId());
        double price = instrument->price(md);
        
        if (std::isnan(price) || std::isinf(price)) {
            throw std::runtime_error("Invalid price in risk metrics calculation");
        }
        
        initial_portfolio_value += price * quantity;
    }
    
    if (std::abs(initial_portfolio_value) < 1e-10) {
        return metrics;  // Return zeros for empty portfolio
    }
    
    // Run Monte Carlo simulations
    std::vector<double> pnl_distribution;
    pnl_distribution.reserve(var_simulations_);
    
    std::mt19937 generator;
    if (use_fixed_seed_) {
        generator.seed(random_seed_);
    } else {
        std::random_device rd;
        generator.seed(rd());
    }
    
    std::normal_distribution<double> distribution(0.0, 1.0);
    const double dt = time_horizon_days_ / 252.0;
    const double sqrt_dt = std::sqrt(dt);
    
    for (int i = 0; i < var_simulations_; ++i) {
        double simulated_portfolio_value = 0.0;
        
        for (const auto& [instrument, quantity] : instruments) {
            const std::string& asset_id = instrument->getAssetId();
            const MarketData& md = cached_market_data.at(asset_id);
            
            const double random_shock = distribution(generator);
            const double drift = (md.risk_free_rate - 0.5 * md.volatility * md.volatility) * dt;
            const double diffusion = md.volatility * sqrt_dt * random_shock;
            const double simulated_spot = md.spot_price * std::exp(drift + diffusion);
            
            if (std::isnan(simulated_spot) || std::isinf(simulated_spot) || simulated_spot <= 0.0) {
                throw std::runtime_error("Invalid simulated spot price in risk metrics calculation");
            }
            
            MarketData simulated_md = md;
            simulated_md.spot_price = simulated_spot;
            
            double simulated_price = instrument->price(simulated_md);
            
            if (std::isnan(simulated_price) || std::isinf(simulated_price)) {
                throw std::runtime_error("Invalid simulated price in risk metrics calculation");
            }
            
            simulated_portfolio_value += simulated_price * quantity;
        }
        
        if (std::isnan(simulated_portfolio_value) || std::isinf(simulated_portfolio_value)) {
            throw std::runtime_error("Invalid simulated portfolio value");
        }
        
        pnl_distribution.push_back(simulated_portfolio_value - initial_portfolio_value);
    }
    
    if (pnl_distribution.empty()) {
        throw std::runtime_error("Risk metrics calculation produced no results");
    }
    
    // Sort the P&L distribution (ascending order: worst losses first)
    std::sort(pnl_distribution.begin(), pnl_distribution.end());
    
    // Calculate VaR at 95% confidence level
    const int index_95 = static_cast<int>((1.0 - 0.95) * var_simulations_);
    if (index_95 < 0 || index_95 >= var_simulations_) {
        throw std::runtime_error("Invalid VaR 95% index calculation");
    }
    metrics.var_95 = -pnl_distribution[index_95];
    
    // Calculate VaR at 99% confidence level
    const int index_99 = static_cast<int>((1.0 - 0.99) * var_simulations_);
    if (index_99 < 0 || index_99 >= var_simulations_) {
        throw std::runtime_error("Invalid VaR 99% index calculation");
    }
    metrics.var_99 = -pnl_distribution[index_99];
    
    // Calculate Expected Shortfall (CVaR) at 95%
    // ES is the average of losses beyond VaR
    double sum_95 = 0.0;
    int count_95 = 0;
    for (int i = 0; i <= index_95; ++i) {
        sum_95 += pnl_distribution[i];
        count_95++;
    }
    if (count_95 > 0) {
        metrics.es_95 = -sum_95 / count_95;
    }
    
    // Calculate Expected Shortfall (CVaR) at 99%
    double sum_99 = 0.0;
    int count_99 = 0;
    for (int i = 0; i <= index_99; ++i) {
        sum_99 += pnl_distribution[i];
        count_99++;
    }
    if (count_99 > 0) {
        metrics.es_99 = -sum_99 / count_99;
    }
    
    return metrics;
}
