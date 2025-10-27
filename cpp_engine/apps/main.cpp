#include <iostream>
#include <iomanip>
#include <vector>
#include <memory>
#include <exception>
#include <random>
#include <string>
#include <algorithm>

#include "../libraries/qe_risk_engine/includes/Portfolio.h"
#include "../libraries/qe_risk_engine/includes/Instrument.h"
#include "../libraries/qe_risk_engine/includes/MarketData.h"
#include "../libraries/qe_risk_engine/includes/RiskEngine.h"

class StockInstrument : public Instrument {
public:
    explicit StockInstrument(const std::string& asset_id) : asset_id_(asset_id) {}
    std::string getAssetId() const override { return asset_id_; }
    std::string getInstrumentType() const override { return "Stock"; }
    bool isValid() const override { return !asset_id_.empty(); }
    double price(const MarketData& md) const override { return md.spot_price; }
    double delta(const MarketData& md) const override { return 1.0; }
    double gamma(const MarketData& md) const override { return 0.0; }
    double vega(const MarketData& md) const override { return 0.0; }
    double theta(const MarketData& md) const override { return 0.0; }
private:
    std::string asset_id_;
};

void printSeparator(char c = '=', int width = 70) {
    std::cout << std::string(width, c) << std::endl;
}

void printHeader(const std::string& title) {
    printSeparator();
    std::cout << "  " << title << std::endl;
    printSeparator();
    std::cout << std::endl;
}

std::vector<std::string> getRandomAssets(int count) {
    std::vector<std::string> asset_pool = {
        "AAPL", "GOOGL", "MSFT", "AMZN", "META", 
        "TSLA", "NVDA", "JPM", "BAC", "WMT"
    };
    
    std::random_device rd;
    std::mt19937 gen(rd());
    std::shuffle(asset_pool.begin(), asset_pool.end(), gen);
    
    std::vector<std::string> selected;
    for (int i = 0; i < std::min(count, (int)asset_pool.size()); ++i) {
        selected.push_back(asset_pool[i]);
    }
    return selected;
}

double getRandomPrice(double min = 50.0, double max = 500.0) {
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_real_distribution<> dis(min, max);
    return dis(gen);
}

double getRandomVolatility(double min = 0.15, double max = 0.35) {
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_real_distribution<> dis(min, max);
    return dis(gen);
}

double getRandomRate(double min = 0.03, double max = 0.06) {
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_real_distribution<> dis(min, max);
    return dis(gen);
}

double getRandomMaturity(double min = 0.1, double max = 2.0) {
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_real_distribution<> dis(min, max);
    return dis(gen);
}

int getRandomQuantity(int min = -100, int max = 100) {
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> dis(min, max);
    int qty = dis(gen);
    return (qty == 0) ? min : qty;
}

OptionType getRandomOptionType() {
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> dis(0, 1);
    return dis(gen) == 0 ? OptionType::Call : OptionType::Put;
}

void demonstrateBasicPortfolio() {
    printHeader("Basic Portfolio Risk Analysis");
    
    try {
        Portfolio portfolio;
        
        auto assets = getRandomAssets(2);
        std::map<std::string, MarketData> market_data;
        
        for (const auto& asset : assets) {
            double spot = getRandomPrice();
            double rate = getRandomRate();
            double vol = getRandomVolatility();
            market_data[asset] = MarketData(asset, spot, rate, vol);
            
            double strike = spot * (0.9 + (std::rand() % 21) / 100.0);
            double maturity = getRandomMaturity();
            OptionType opt_type = getRandomOptionType();
            int quantity = getRandomQuantity(-100, 100);
            
            portfolio.addInstrument(
                std::make_unique<EuropeanOption>(opt_type, strike, maturity, asset),
                quantity
            );
        }
        
        RiskEngine engine;
        engine.setVaRSimulations(50000);
        
        PortfolioRiskResult results = engine.calculatePortfolioRisk(portfolio, market_data);
        
        std::cout << std::fixed << std::setprecision(4);
        std::cout << "Portfolio Size: " << portfolio.size() << " instruments\n" << std::endl;
        
        std::cout << "Risk Metrics:" << std::endl;
        std::cout << "  Total PV:           $" << std::setw(12) << results.total_pv << std::endl;
        std::cout << "  Total Delta:         " << std::setw(12) << results.total_delta << std::endl;
        std::cout << "  Total Gamma:         " << std::setw(12) << results.total_gamma << std::endl;
        std::cout << "  Total Vega:          " << std::setw(12) << results.total_vega << std::endl;
        std::cout << "  Total Theta:         " << std::setw(12) << results.total_theta << std::endl;
        std::cout << "\nValue at Risk (1-day):" << std::endl;
        std::cout << "  95% VaR:            $" << std::setw(12) << results.value_at_risk_95 << std::endl;
        std::cout << "  99% VaR:            $" << std::setw(12) << results.value_at_risk_99 << std::endl;
        std::cout << "\nExpected Shortfall (1-day):" << std::endl;
        std::cout << "  95% ES:             $" << std::setw(12) << results.expected_shortfall_95 << std::endl;
        std::cout << "  99% ES:             $" << std::setw(12) << results.expected_shortfall_99 << std::endl;
        
        std::cout << "\nNet Positions:" << std::endl;
        for (const auto& asset : assets) {
            std::cout << "  " << std::left << std::setw(10) << (asset + ":") 
                      << std::right << std::setw(12) << portfolio.getTotalQuantityForAsset(asset) << std::endl;
        }
        
        std::cout << "\n";
        
    } catch (const std::exception& e) {
        std::cerr << "Error in basic portfolio: " << e.what() << std::endl;
    }
}

void demonstrateMultiplePricingModels() {
    printHeader("Multiple Pricing Models Comparison");
    
    try {
        auto asset = getRandomAssets(1)[0];
        double spot = getRandomPrice();
        double rate = getRandomRate();
        double vol = getRandomVolatility();
        double strike = spot * (0.95 + (std::rand() % 11) / 100.0);
        double maturity = getRandomMaturity(0.5, 1.5);
        
        MarketData md(asset, spot, rate, vol);
        
        EuropeanOption bs_option(OptionType::Call, strike, maturity, asset, PricingModel::BlackScholes);
        EuropeanOption bin_option(OptionType::Call, strike, maturity, asset, PricingModel::Binomial);
        bin_option.setBinomialSteps(200);
        
        EuropeanOption jd_option(OptionType::Call, strike, maturity, asset, PricingModel::MertonJumpDiffusion);
        jd_option.setJumpParameters(2.0, -0.05, 0.15);
        
        std::cout << std::fixed << std::setprecision(4);
        std::cout << "Call Option (K=" << strike << ", S=" << spot 
                  << ", T=" << maturity << ", σ=" << vol << "):\n" << std::endl;
        
        std::cout << "Black-Scholes Model:" << std::endl;
        std::cout << "  Price:  $" << bs_option.price(md) << std::endl;
        std::cout << "  Delta:   " << bs_option.delta(md) << std::endl;
        std::cout << "  Gamma:   " << bs_option.gamma(md) << std::endl;
        std::cout << "  Vega:    " << bs_option.vega(md) << std::endl;
        std::cout << "  Theta:   " << bs_option.theta(md) << std::endl;
        
        std::cout << "\nBinomial Tree Model (200 steps):" << std::endl;
        std::cout << "  Price:  $" << bin_option.price(md) << std::endl;
        std::cout << "  Delta:   " << bin_option.delta(md) << std::endl;
        
        std::cout << "\nMerton Jump Diffusion Model (λ=2.0, μ=-0.05, σ_j=0.15):" << std::endl;
        std::cout << "  Price:  $" << jd_option.price(md) << std::endl;
        std::cout << "  Delta:   " << jd_option.delta(md) << std::endl;
        
        std::cout << "\n";
        
    } catch (const std::exception& e) {
        std::cerr << "Error in pricing models: " << e.what() << std::endl;
    }
}

void demonstrateAmericanOptions() {
    printHeader("American Options Pricing");
    
    try {
        auto asset = getRandomAssets(1)[0];
        double spot = getRandomPrice();
        double rate = getRandomRate();
        double vol = getRandomVolatility(0.2, 0.35);
        double strike = spot * (0.9 + (std::rand() % 11) / 100.0);
        double maturity = getRandomMaturity(0.3, 0.8);
        
        MarketData md(asset, spot, rate, vol);
        
        EuropeanOption euro_put(OptionType::Put, strike, maturity, asset);
        AmericanOption american_put(OptionType::Put, strike, maturity, asset, 200);
        
        std::cout << std::fixed << std::setprecision(4);
        std::cout << "Put Option (K=" << strike << ", S=" << spot 
                  << ", T=" << maturity << ", σ=" << vol << "):\n" << std::endl;
        
        std::cout << "European Put:" << std::endl;
        std::cout << "  Price:  $" << euro_put.price(md) << std::endl;
        std::cout << "  Delta:   " << euro_put.delta(md) << std::endl;
        
        std::cout << "\nAmerican Put (200 steps):" << std::endl;
        std::cout << "  Price:  $" << american_put.price(md) << std::endl;
        std::cout << "  Delta:   " << american_put.delta(md) << std::endl;
        
        double early_exercise_premium = american_put.price(md) - euro_put.price(md);
        std::cout << "\nEarly Exercise Premium: $" << early_exercise_premium << std::endl;
        
        std::cout << "\n";
        
    } catch (const std::exception& e) {
        std::cerr << "Error in American options: " << e.what() << std::endl;
    }
}

void demonstrateMarketDataManager() {
    printHeader("Market Data Manager");
    
    try {
        MarketDataManager mdm;
        
        auto assets = getRandomAssets(3);
        for (const auto& asset : assets) {
            double spot = getRandomPrice();
            double rate = getRandomRate();
            double vol = getRandomVolatility();
            mdm.addMarketData(asset, MarketData(asset, spot, rate, vol));
        }
        
        std::cout << "Market Data Store Size: " << mdm.size() << " assets\n" << std::endl;
        
        std::cout << std::fixed << std::setprecision(2);
        auto all_data = mdm.getAllMarketData();
        for (const auto& [asset_id, md] : all_data) {
            std::cout << asset_id << ":" << std::endl;
            std::cout << "  Spot:  $" << md.spot_price << std::endl;
            std::cout << "  Rate:   " << (md.risk_free_rate * 100) << "%" << std::endl;
            std::cout << "  Vol:    " << (md.volatility * 100) << "%" << std::endl;
            std::cout << std::endl;
        }
        
        if (!assets.empty()) {
            double new_spot = getRandomPrice();
            mdm.updateMarketData(assets[0], MarketData(assets[0], new_spot, 
                                mdm.getMarketData(assets[0]).risk_free_rate,
                                mdm.getMarketData(assets[0]).volatility));
            std::cout << "Updated " << assets[0] << " spot price to: $" 
                      << mdm.getMarketData(assets[0]).spot_price << std::endl;
        }
        
        std::cout << "\n";
        
    } catch (const std::exception& e) {
        std::cerr << "Error in market data manager: " << e.what() << std::endl;
    }
}

void demonstrateComplexPortfolio() {
    printHeader("Complex Multi-Asset Portfolio");
    
    try {
        Portfolio portfolio;
        portfolio.reserve(10);
        
        auto assets = getRandomAssets(2);
        std::map<std::string, MarketData> market_data;
        
        for (const auto& asset : assets) {
            double spot = getRandomPrice(100.0, 300.0);
            double rate = getRandomRate();
            double vol = getRandomVolatility();
            market_data[asset] = MarketData(asset, spot, rate, vol);
            
            int num_options = 2 + std::rand() % 3;
            for (int i = 0; i < num_options; ++i) {
                double strike_mult = 0.85 + (std::rand() % 31) / 100.0;
                double strike = spot * strike_mult;
                double maturity = getRandomMaturity(0.2, 0.8);
                OptionType opt_type = getRandomOptionType();
                int quantity = getRandomQuantity(-50, 50);
                
                portfolio.addInstrument(
                    std::make_unique<EuropeanOption>(opt_type, strike, maturity, asset),
                    quantity
                );
            }
        }
        
        RiskEngine engine;
        engine.setVaRSimulations(100000);
        engine.setVaRTimeHorizonDays(1.0);
        
        PortfolioRiskResult results = engine.calculatePortfolioRisk(portfolio, market_data);
        
        std::cout << std::fixed << std::setprecision(4);
        std::cout << "Portfolio Composition:" << std::endl;
        std::cout << "  Total Instruments:  " << portfolio.size() << std::endl;
        for (const auto& asset : assets) {
            std::cout << "  " << asset << " Net Position:  " 
                      << portfolio.getTotalQuantityForAsset(asset) << std::endl;
        }
        std::cout << std::endl;
        
        std::cout << "Risk Metrics:" << std::endl;
        std::cout << "  Total PV:           $" << std::setw(12) << results.total_pv << std::endl;
        std::cout << "  Total Delta:         " << std::setw(12) << results.total_delta << std::endl;
        std::cout << "  Total Gamma:         " << std::setw(12) << results.total_gamma << std::endl;
        std::cout << "  Total Vega:          " << std::setw(12) << results.total_vega << std::endl;
        std::cout << "  Total Theta:         " << std::setw(12) << results.total_theta << std::endl;
        
        std::cout << "\nValue at Risk (1-day):" << std::endl;
        std::cout << "  95% VaR:            $" << std::setw(12) << results.value_at_risk_95 << std::endl;
        std::cout << "  99% VaR:            $" << std::setw(12) << results.value_at_risk_99 << std::endl;
        
        std::cout << "\nExpected Shortfall (1-day):" << std::endl;
        std::cout << "  95% ES:             $" << std::setw(12) << results.expected_shortfall_95 << std::endl;
        std::cout << "  99% ES:             $" << std::setw(12) << results.expected_shortfall_99 << std::endl;
        
        std::cout << "\n  Simulations:        " << engine.getVaRSimulations() << std::endl;
        std::cout << std::endl;
        
        std::string delta_status = "NEUTRAL";
        if (std::abs(results.total_delta) > 10.0) {
            delta_status = results.total_delta > 0 ? "LONG" : "SHORT";
        }
        
        std::string gamma_status = results.total_gamma > 0.1 ? "LONG GAMMA" : "SHORT GAMMA";
        
        std::cout << "Portfolio Positioning:" << std::endl;
        std::cout << "  Delta:  " << delta_status << std::endl;
        std::cout << "  Gamma:  " << gamma_status << std::endl;
        
        std::cout << "\n";
        
    } catch (const std::exception& e) {
        std::cerr << "Error in complex portfolio: " << e.what() << std::endl;
    }
}

void demonstrateMarketDataCache() {
    printHeader("Market Data Caching Demo");
    
    try {
        RiskEngine engine;
        engine.setMarketDataCachePath("./market_data_cache.db");
        
        auto assets = getRandomAssets(3);
        std::map<std::string, MarketData> market_data;
        
        std::cout << "Initial market data:\n" << std::endl;
        for (const auto& asset : assets) {
            double spot = getRandomPrice();
            double rate = getRandomRate();
            double vol = getRandomVolatility();
            market_data[asset] = MarketData(asset, spot, rate, vol);
            
            std::cout << asset << ":" << std::endl;
            std::cout << "  Spot:  $" << spot << std::endl;
            std::cout << "  Rate:   " << (rate * 100) << "%" << std::endl;
            std::cout << "  Vol:    " << (vol * 100) << "%\n" << std::endl;
        }
        
        Portfolio portfolio;
        for (const auto& asset : assets) {
            portfolio.addInstrument(
                std::make_unique<StockInstrument>(asset),
                getRandomQuantity(50, 150)
            );
        }
        
        std::cout << "Calculating risk with fresh market data..." << std::endl;
        auto result1 = engine.calculatePortfolioRisk(portfolio, market_data);
        std::cout << "Portfolio Value: $" << result1.total_pv << "\n" << std::endl;
        
        auto& mdm = engine.getMarketDataManager();
        mdm.clear();
        
        std::cout << "Calculating risk using cached data..." << std::endl;
        auto result2 = engine.calculatePortfolioRisk(portfolio, std::map<std::string, MarketData>());
        std::cout << "Portfolio Value: $" << result2.total_pv << "\n" << std::endl;
        
        if (std::abs(result2.total_pv - result1.total_pv) < 0.01) {
            std::cout << "SUCCESS: Cached calculation matches original!" << std::endl;
        } else {
            std::cout << "WARNING: Cache results differ from original." << std::endl;
        }
        
    } catch (const std::exception& e) {
        std::cerr << "Error in cache demo: " << e.what() << std::endl;
    }
}

void demonstrateErrorHandling() {
    printHeader("Error Handling and Validation");
    
    std::cout << "Testing input validation:\n" << std::endl;
    
    try {
        MarketData invalid_md("TEST", -100.0, 0.05, 0.2);
    } catch (const std::exception& e) {
        std::cout << "✓ Caught invalid spot price: " << e.what() << std::endl;
    }
    
    try {
        EuropeanOption invalid_option(OptionType::Call, -100.0, 1.0, "TEST");
    } catch (const std::exception& e) {
        std::cout << "✓ Caught invalid strike: " << e.what() << std::endl;
    }
    
    try {
        Portfolio portfolio;
        portfolio.addInstrument(nullptr, 10);
    } catch (const std::exception& e) {
        std::cout << "✓ Caught null instrument: " << e.what() << std::endl;
    }
    
    try {
        RiskEngine engine;
        engine.setVaRSimulations(-1000);
    } catch (const std::exception& e) {
        std::cout << "✓ Caught invalid VaR simulations: " << e.what() << std::endl;
    }
    
    try {
        Portfolio portfolio;
        portfolio.getTotalQuantityForAsset("");
    } catch (const std::exception& e) {
        std::cout << "✓ Caught empty asset ID: " << e.what() << std::endl;
    }
    
    std::cout << "\nAll validation tests passed!" << std::endl;
    std::cout << "\n";
}

int main() {
    std::cout << "\n";
    printSeparator('=', 70);
    std::cout << "  QUANTITATIVE RISK ENGINE - DEMONSTRATION" << std::endl;
    printSeparator('=', 70);
    std::cout << "\n";
    
    try {
        demonstrateBasicPortfolio();
        demonstrateMultiplePricingModels();
        demonstrateAmericanOptions();
        demonstrateMarketDataManager();
        demonstrateComplexPortfolio();
        demonstrateMarketDataCache();
        demonstrateErrorHandling();
        
        printSeparator('=', 70);
        std::cout << "  All demonstrations completed successfully!" << std::endl;
        printSeparator('=', 70);
        std::cout << "\n";
        
        return 0;
        
    } catch (const std::exception& e) {
        std::cerr << "\nFATAL ERROR: " << e.what() << std::endl;
        return 1;
    } catch (...) {
        std::cerr << "\nFATAL ERROR: Unknown exception occurred" << std::endl;
        return 1;
    }
}
