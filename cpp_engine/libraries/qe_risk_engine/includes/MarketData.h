#ifndef MARKETDATA_H
#define MARKETDATA_H

#include <string>
#include <stdexcept>
#include <cmath>
#include <map>
class LocalMarketDB; // forward declaration to avoid circular include

struct MarketData {
    std::string asset_id;
    double spot_price;
    double risk_free_rate;
    double volatility;
    double dividend_yield;
    
    MarketData()
        : asset_id(""),
          spot_price(0.0),
          risk_free_rate(0.0),
          volatility(0.0),
          dividend_yield(0.0) {}
    
    MarketData(std::string id, double spot, double rate, double vol)
        : asset_id(id),
          spot_price(spot),
          risk_free_rate(rate),
          volatility(vol),
          dividend_yield(0.0) {
        validate();
    }
    
    MarketData(std::string id, double spot, double rate, double vol, double div)
        : asset_id(id),
          spot_price(spot),
          risk_free_rate(rate),
          volatility(vol),
          dividend_yield(div) {
        validate();
    }
    
    void validate() const {
        if (asset_id.empty()) {
            throw std::invalid_argument("Market data asset ID cannot be empty");
        }
        if (spot_price <= 0.0) {
            throw std::invalid_argument("Spot price must be positive for " + asset_id);
        }
        if (volatility < 0.0) {
            throw std::invalid_argument("Volatility cannot be negative for " + asset_id);
        }
        if (dividend_yield < 0.0) {
            throw std::invalid_argument("Dividend yield cannot be negative for " + asset_id);
        }
        if (std::isnan(spot_price) || std::isinf(spot_price)) {
            throw std::invalid_argument("Invalid spot price for " + asset_id);
        }
        if (std::isnan(risk_free_rate) || std::isinf(risk_free_rate)) {
            throw std::invalid_argument("Invalid risk-free rate for " + asset_id);
        }
        if (std::isnan(volatility) || std::isinf(volatility)) {
            throw std::invalid_argument("Invalid volatility for " + asset_id);
        }
        if (std::isnan(dividend_yield) || std::isinf(dividend_yield)) {
            throw std::invalid_argument("Invalid dividend yield for " + asset_id);
        }
    }
    
    bool isValid() const {
        try {
            validate();
            return true;
        } catch (...) {
            return false;
        }
    }
};

class MarketDataManager {
public:
    MarketDataManager();
    void addMarketData(const std::string& asset_id, const MarketData& md);
    void updateMarketData(const std::string& asset_id, const MarketData& md);
    MarketData getMarketData(const std::string& asset_id) const;
    bool hasMarketData(const std::string& asset_id) const;
    void removeMarketData(const std::string& asset_id);
    void clear();
    size_t size() const;
    std::map<std::string, MarketData> getAllMarketData() const;
    void setLocalDB(LocalMarketDB* db);
    
private:
    std::map<std::string, MarketData> market_data_map_;
    LocalMarketDB* local_db_{nullptr};
};

#endif