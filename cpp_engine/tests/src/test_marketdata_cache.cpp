#include <iostream>
#include "../../libraries/qe_risk_engine/includes/MarketData.h"
#include "../../libraries/qe_risk_engine/includes/LocalMarketDB.h"

int main() {
    const std::string db_path = "./test_marketdata_cache.db";
    LocalMarketDB db(db_path);
    if (!db.init()) {
        std::cerr << "Failed to init local DB" << std::endl;
        return 1;
    }

    MarketData md("TEST_ASSET", 100.5, 0.01, 0.2, 0.0);
    if (!db.saveMarketData(md)) {
        std::cerr << "Failed to save market data" << std::endl;
        return 2;
    }

    MarketData loaded;
    if (!db.loadMarketData("TEST_ASSET", loaded)) {
        std::cerr << "Failed to load market data" << std::endl;
        return 3;
    }

    if (loaded.asset_id != md.asset_id || loaded.spot_price != md.spot_price) {
        std::cerr << "Loaded data doesn't match saved data" << std::endl;
        return 4;
    }

    std::cout << "LocalMarketDB basic save/load PASSED" << std::endl;
    return 0;
}
