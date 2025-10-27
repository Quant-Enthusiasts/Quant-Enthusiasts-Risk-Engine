#include "MarketData.h"
#include "LocalMarketDB.h"
#include <utility>

MarketDataManager::MarketDataManager() : local_db_(nullptr) {}

void MarketDataManager::setLocalDB(LocalMarketDB* db) {
    local_db_ = db;
}

void MarketDataManager::addMarketData(const std::string& asset_id, const MarketData& md) {
    if (asset_id.empty()) {
        throw std::invalid_argument("Asset ID cannot be empty");
    }
    
    md.validate();
    
    if (market_data_map_.find(asset_id) != market_data_map_.end()) {
        throw std::runtime_error("Market data for " + asset_id + " already exists. Use updateMarketData instead.");
    }
    
    market_data_map_[asset_id] = md;
    // Persist to local DB if available
    if (local_db_) {
        local_db_->saveMarketData(md);
    }
}

void MarketDataManager::updateMarketData(const std::string& asset_id, const MarketData& md) {
    if (asset_id.empty()) {
        throw std::invalid_argument("Asset ID cannot be empty");
    }
    
    md.validate();
    
    if (market_data_map_.find(asset_id) == market_data_map_.end()) {
        throw std::runtime_error("Market data for " + asset_id + " does not exist. Use addMarketData instead.");
    }
    
    market_data_map_[asset_id] = md;
    if (local_db_) {
        local_db_->saveMarketData(md);
    }
}

MarketData MarketDataManager::getMarketData(const std::string& asset_id) const {
    if (asset_id.empty()) {
        throw std::invalid_argument("Asset ID cannot be empty");
    }
    
    auto it = market_data_map_.find(asset_id);
    if (it == market_data_map_.end()) {
        // Try loading from local DB if available
        if (local_db_) {
            MarketData md;
            if (local_db_->loadMarketData(asset_id, md)) {
                auto non_const_this = const_cast<MarketDataManager*>(this);
                non_const_this->market_data_map_[asset_id] = md;
                return md;
            }
        }
        throw std::runtime_error("Market data for " + asset_id + " not found");
    }
    return it->second;
}

bool MarketDataManager::hasMarketData(const std::string& asset_id) const {
    return market_data_map_.find(asset_id) != market_data_map_.end();
}

void MarketDataManager::removeMarketData(const std::string& asset_id) {
    if (asset_id.empty()) {
        throw std::invalid_argument("Asset ID cannot be empty");
    }
    
    auto it = market_data_map_.find(asset_id);
    if (it == market_data_map_.end()) {
        throw std::runtime_error("Market data for " + asset_id + " not found");
    }
    
    market_data_map_.erase(it);
    if (local_db_) {
        local_db_->removeMarketData(asset_id);
    }
}

void MarketDataManager::clear() {
    market_data_map_.clear();
}

size_t MarketDataManager::size() const {
    return market_data_map_.size();
}

std::map<std::string, MarketData> MarketDataManager::getAllMarketData() const {
    return market_data_map_;
}