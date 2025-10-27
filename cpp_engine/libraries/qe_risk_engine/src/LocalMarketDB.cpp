#include "LocalMarketDB.h"
#include <sqlite3.h>
#include <iostream>

LocalMarketDB::LocalMarketDB(const std::string& db_path)
    : db_path_(db_path), db_handle_(nullptr) {}

LocalMarketDB::~LocalMarketDB() {
    if (db_handle_) {
        sqlite3* db = reinterpret_cast<sqlite3*>(db_handle_);
        sqlite3_close(db);
        db_handle_ = nullptr;
    }
}

bool LocalMarketDB::init() {
    sqlite3* db = nullptr;
    if (sqlite3_open(db_path_.c_str(), &db) != SQLITE_OK) {
        std::cerr << "Failed to open local market DB: " << sqlite3_errmsg(db) << std::endl;
        if (db) sqlite3_close(db);
        return false;
    }

    const char* create_table_sql =
        "CREATE TABLE IF NOT EXISTS market_data ("
        "asset_id TEXT PRIMARY KEY,"
        "spot_price REAL,"
        "risk_free_rate REAL,"
        "volatility REAL,"
        "dividend_yield REAL,"
        "last_updated INTEGER"
        ");";

    char* err = nullptr;
    if (sqlite3_exec(db, create_table_sql, nullptr, nullptr, &err) != SQLITE_OK) {
        std::cerr << "Failed to create market_data table: " << (err ? err : "unknown") << std::endl;
        if (err) sqlite3_free(err);
        sqlite3_close(db);
        return false;
    }

    db_handle_ = reinterpret_cast<void*>(db);
    return true;
}

bool LocalMarketDB::saveMarketData(const MarketData& md) {
    if (!db_handle_) return false;
    sqlite3* db = reinterpret_cast<sqlite3*>(db_handle_);

    const char* upsert_sql =
        "INSERT INTO market_data(asset_id, spot_price, risk_free_rate, volatility, dividend_yield, last_updated)"
        " VALUES(?,?,?,?,?,?)"
        " ON CONFLICT(asset_id) DO UPDATE SET"
        " spot_price=excluded.spot_price,"
        " risk_free_rate=excluded.risk_free_rate,"
        " volatility=excluded.volatility,"
        " dividend_yield=excluded.dividend_yield,"
        " last_updated=excluded.last_updated;";

    sqlite3_stmt* stmt = nullptr;
    if (sqlite3_prepare_v2(db, upsert_sql, -1, &stmt, nullptr) != SQLITE_OK) {
        std::cerr << "Failed to prepare upsert statement: " << sqlite3_errmsg(db) << std::endl;
        return false;
    }

    sqlite3_bind_text(stmt, 1, md.asset_id.c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_double(stmt, 2, md.spot_price);
    sqlite3_bind_double(stmt, 3, md.risk_free_rate);
    sqlite3_bind_double(stmt, 4, md.volatility);
    sqlite3_bind_double(stmt, 5, md.dividend_yield);
    sqlite3_bind_int64(stmt, 6, static_cast<sqlite3_int64>(std::time(nullptr)));

    int rc = sqlite3_step(stmt);
    sqlite3_finalize(stmt);
    if (rc != SQLITE_DONE) {
        std::cerr << "Failed to execute upsert: " << sqlite3_errmsg(db) << std::endl;
        return false;
    }
    return true;
}

bool LocalMarketDB::loadMarketData(const std::string& asset_id, MarketData& md) const {
    if (!db_handle_) return false;
    sqlite3* db = reinterpret_cast<sqlite3*>(db_handle_);

    const char* select_sql =
        "SELECT spot_price, risk_free_rate, volatility, dividend_yield FROM market_data WHERE asset_id = ? LIMIT 1;";

    sqlite3_stmt* stmt = nullptr;
    if (sqlite3_prepare_v2(db, select_sql, -1, &stmt, nullptr) != SQLITE_OK) {
        std::cerr << "Failed to prepare select statement: " << sqlite3_errmsg(db) << std::endl;
        return false;
    }

    sqlite3_bind_text(stmt, 1, asset_id.c_str(), -1, SQLITE_TRANSIENT);

    int rc = sqlite3_step(stmt);
    if (rc == SQLITE_ROW) {
        md.asset_id = asset_id;
        md.spot_price = sqlite3_column_double(stmt, 0);
        md.risk_free_rate = sqlite3_column_double(stmt, 1);
        md.volatility = sqlite3_column_double(stmt, 2);
        md.dividend_yield = sqlite3_column_double(stmt, 3);
        sqlite3_finalize(stmt);
        return true;
    }

    sqlite3_finalize(stmt);
    return false;
}

bool LocalMarketDB::removeMarketData(const std::string& asset_id) {
    if (!db_handle_) return false;
    sqlite3* db = reinterpret_cast<sqlite3*>(db_handle_);

    const char* delete_sql = "DELETE FROM market_data WHERE asset_id = ?;";
    sqlite3_stmt* stmt = nullptr;
    if (sqlite3_prepare_v2(db, delete_sql, -1, &stmt, nullptr) != SQLITE_OK) {
        std::cerr << "Failed to prepare delete statement: " << sqlite3_errmsg(db) << std::endl;
        return false;
    }

    sqlite3_bind_text(stmt, 1, asset_id.c_str(), -1, SQLITE_TRANSIENT);
    int rc = sqlite3_step(stmt);
    sqlite3_finalize(stmt);
    return rc == SQLITE_DONE;
}
