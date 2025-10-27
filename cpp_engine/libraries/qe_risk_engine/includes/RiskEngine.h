#ifndef RISKENGINE_H
#define RISKENGINE_H

#include "Portfolio.h"
#include "MarketData.h"
#include "LocalMarketDB.h"
#include <memory>
#include <map>
#include <vector>
#include <string>
#include <stdexcept>

struct PortfolioRiskResult {
    double total_pv = 0.0;
    double total_delta = 0.0;
    double total_gamma = 0.0;
    double total_vega = 0.0;
    double total_theta = 0.0;
    double value_at_risk_95 = 0.0;
    double value_at_risk_99 = 0.0;
    double expected_shortfall_95 = 0.0;
    double expected_shortfall_99 = 0.0;
    
    void reset() {
        total_pv = 0.0;
        total_delta = 0.0;
        total_gamma = 0.0;
        total_vega = 0.0;
        total_theta = 0.0;
        value_at_risk_95 = 0.0;
        value_at_risk_99 = 0.0;
        expected_shortfall_95 = 0.0;
        expected_shortfall_99 = 0.0;
    }
    
    bool isValid() const {
        return !std::isnan(total_pv) && !std::isnan(total_delta) && 
               !std::isnan(total_gamma) && !std::isnan(total_vega) && 
               !std::isnan(total_theta) && !std::isnan(value_at_risk_95) &&
               !std::isnan(value_at_risk_99) && !std::isnan(expected_shortfall_95) &&
               !std::isnan(expected_shortfall_99) &&
               !std::isinf(total_pv) && !std::isinf(total_delta) && 
               !std::isinf(total_gamma) && !std::isinf(total_vega) && 
               !std::isinf(total_theta) && !std::isinf(value_at_risk_95) &&
               !std::isinf(value_at_risk_99) && !std::isinf(expected_shortfall_95) &&
               !std::isinf(expected_shortfall_99);
    }
};

struct RiskMetrics {
    double var_95 = 0.0;
    double var_99 = 0.0;
    double es_95 = 0.0;
    double es_99 = 0.0;
};

class RiskEngine {
public:
    RiskEngine();
    explicit RiskEngine(int var_simulations);
    
    PortfolioRiskResult calculatePortfolioRisk(
        const Portfolio& portfolio, 
        const std::map<std::string, MarketData>& market_data_map
    );
    
    void setVaRSimulations(int simulations);
    int getVaRSimulations() const;
    
    void setVaRTimeHorizonDays(double days);
    double getVaRTimeHorizonDays() const;
    
    void setRandomSeed(unsigned int seed);
    void setUseFixedSeed(bool use_fixed);
    
    // Config database path for market data caching
    void setMarketDataCachePath(const std::string& db_path);
    
    const MarketDataManager& getMarketDataManager() const { return market_data_manager_; }
    MarketDataManager& getMarketDataManager() { return market_data_manager_; }

private:
    int var_simulations_;
    double time_horizon_days_;
    unsigned int random_seed_;
    bool use_fixed_seed_;
    MarketDataManager market_data_manager_;
    std::unique_ptr<LocalMarketDB> local_market_db_;
    std::string market_data_cache_path_;
    
    RiskMetrics calculateRiskMetrics(
        const Portfolio& portfolio, 
        const std::map<std::string, MarketData>& market_data_map
    );
    
    void validateMarketData(
        const Portfolio& portfolio,
        const std::map<std::string, MarketData>& market_data_map
    ) const;
    
    void validateParameters() const;
    
    double calculateSingleInstrumentMetric(
        const std::unique_ptr<Instrument>& instrument,
        int quantity,
        const MarketData& md,
        const std::string& metric_name
    ) const;
};

#endif
