#ifndef LOCALMARKETDB_H
#define LOCALMARKETDB_H

#include <string>
#include <ctime>
#include "MarketData.h"

// Lightweight SQLite-backed local cache for market data.
class LocalMarketDB {
public:
    explicit LocalMarketDB(const std::string& db_path);
    ~LocalMarketDB();

    bool init();

    bool saveMarketData(const MarketData& md);
    bool loadMarketData(const std::string& asset_id, MarketData& md) const;
    bool removeMarketData(const std::string& asset_id);

private:
    std::string db_path_;
    void* db_handle_;
};

#endif
