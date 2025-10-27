#include <iostream>
#include <memory>
#include <filesystem>
#include "../../libraries/qe_risk_engine/includes/RiskEngine.h"
#include "../../libraries/qe_risk_engine/includes/Portfolio.h"

// Simple stock instrument for testing
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

bool testOfflineMarketData() {
    const std::string test_db = "./test_market_integration.db";
    std::filesystem::remove(test_db); // Start fresh
    
    // Create RiskEngine and configure cache
    RiskEngine engine;
    engine.setMarketDataCachePath(test_db);
    
    // Create test portfolio with a stock
    Portfolio portfolio;
    portfolio.addInstrument(std::make_unique<StockInstrument>("TEST_STOCK"), 100);
    
    // Initial market data
    std::map<std::string, MarketData> market_data;
    market_data["TEST_STOCK"] = MarketData("TEST_STOCK", 150.0, 0.02, 0.25);
    
    // Calculate risk - this should persist market data to cache
    auto result1 = engine.calculatePortfolioRisk(portfolio, market_data);
    
    // Verify initial calculation
    if (std::abs(result1.total_pv - (150.0 * 100)) > 0.01) {
        std::cerr << "Initial PV calculation incorrect" << std::endl;
        return false;
    }
    
    // Get a reference to market data manager and clear in-memory cache
    auto& mdm = engine.getMarketDataManager();
    mdm.clear();
    
    // Try to get the data back from cache through manager API
    try {
        const MarketData& cached = mdm.getMarketData("TEST_STOCK");
        if (std::abs(cached.spot_price - 150.0) > 0.01) {
            std::cerr << "Cached spot price mismatch" << std::endl;
            return false;
        }
    } catch (const std::exception& e) {
        std::cerr << "Failed to get cached data: " << e.what() << std::endl;
        return false;
    }
    
    // Now calculate risk using just the cached data (no new market data provided)
    auto result2 = engine.calculatePortfolioRisk(portfolio, std::map<std::string, MarketData>());
    
    // Verify we got the same result using cached data
    if (std::abs(result2.total_pv - result1.total_pv) > 0.01) {
        std::cerr << "Cached data calculation mismatch: "
                  << result2.total_pv << " != " << result1.total_pv << std::endl;
        return false;
    }
    
    std::cout << "Market data cache integration test PASSED" << std::endl;
    return true;
}

int main() {
    if (!testOfflineMarketData()) {
        return 1;
    }
    return 0;
}