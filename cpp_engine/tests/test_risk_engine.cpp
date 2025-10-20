#include "simple_test.h"
#include "../src/RiskEngine.h"
#include "../src/Portfolio.h"
#include "../src/Instrument.h"
#include "../src/MarketData.h"
#include <memory>
#include <map>
#include <cmath>

// Helper function to create market data
MarketData createMarketData(const std::string& asset_id, double spot, double rate, double vol) {
    MarketData md;
    md.asset_id = asset_id;
    md.spot_price = spot;
    md.risk_free_rate = rate;
    md.volatility = vol;
    return md;
}

void test_empty_portfolio(TestSuite& suite) {
    suite.run_test("Empty portfolio returns zero metrics", [&]() {
        Portfolio portfolio;
        std::map<std::string, MarketData> market_data_map;
        
        RiskEngine engine;
        PortfolioRiskResult result = engine.calculatePortfolioRisk(portfolio, market_data_map);
        
        suite.assert_equal(0.0, result.total_pv, 1e-10);
        suite.assert_equal(0.0, result.total_delta, 1e-10);
        suite.assert_equal(0.0, result.total_gamma, 1e-10);
        suite.assert_equal(0.0, result.total_vega, 1e-10);
        suite.assert_equal(0.0, result.total_theta, 1e-10);
        suite.assert_equal(0.0, result.value_at_risk_95, 1e-10);
    });
}

void test_single_call_option(TestSuite& suite) {
    suite.run_test("Single ATM call option", [&]() {
        Portfolio portfolio;
        portfolio.addInstrument(
            std::make_unique<EuropeanOption>(OptionType::Call, 100.0, 1.0, "AAPL"),
            1
        );
        
        std::map<std::string, MarketData> market_data_map;
        market_data_map["AAPL"] = createMarketData("AAPL", 100.0, 0.05, 0.2);
        
        RiskEngine engine;
        PortfolioRiskResult result = engine.calculatePortfolioRisk(portfolio, market_data_map);
        
        // Expected values from BlackScholes
        suite.assert_equal(10.4506, result.total_pv, 0.01, "PV");
        suite.assert_equal(0.6368, result.total_delta, 0.01, "Delta");
        suite.assert_equal(0.0188, result.total_gamma, 0.001, "Gamma");
        suite.assert_equal(37.5245, result.total_vega, 0.1, "Vega");
        
        // VaR should be positive (loss is positive)
        if (result.value_at_risk_95 <= 0.0) {
            throw std::runtime_error("VaR should be positive for long position");
        }
    });
}

void test_single_put_option(TestSuite& suite) {
    suite.run_test("Single ATM put option", [&]() {
        Portfolio portfolio;
        portfolio.addInstrument(
            std::make_unique<EuropeanOption>(OptionType::Put, 100.0, 1.0, "AAPL"),
            1
        );
        
        std::map<std::string, MarketData> market_data_map;
        market_data_map["AAPL"] = createMarketData("AAPL", 100.0, 0.05, 0.2);
        
        RiskEngine engine;
        PortfolioRiskResult result = engine.calculatePortfolioRisk(portfolio, market_data_map);
        
        suite.assert_equal(5.5735, result.total_pv, 0.01, "PV");
        suite.assert_equal(-0.3632, result.total_delta, 0.01, "Delta");
        suite.assert_equal(0.0188, result.total_gamma, 0.001, "Gamma");
        suite.assert_equal(37.5245, result.total_vega, 0.1, "Vega");
    });
}

void test_quantity_scaling(TestSuite& suite) {
    suite.run_test("Greeks scale with quantity", [&]() {
        Portfolio portfolio;
        portfolio.addInstrument(
            std::make_unique<EuropeanOption>(OptionType::Call, 100.0, 1.0, "AAPL"),
            10  // 10 contracts
        );
        
        std::map<std::string, MarketData> market_data_map;
        market_data_map["AAPL"] = createMarketData("AAPL", 100.0, 0.05, 0.2);
        
        RiskEngine engine;
        PortfolioRiskResult result = engine.calculatePortfolioRisk(portfolio, market_data_map);
        
        // Should be 10x the single option values
        suite.assert_equal(104.506, result.total_pv, 0.1, "PV scaled by 10");
        suite.assert_equal(6.368, result.total_delta, 0.01, "Delta scaled by 10");
        suite.assert_equal(0.188, result.total_gamma, 0.001, "Gamma scaled by 10");
        suite.assert_equal(375.245, result.total_vega, 1.0, "Vega scaled by 10");
    });
}

void test_negative_quantity(TestSuite& suite) {
    suite.run_test("Negative quantity (short position)", [&]() {
        Portfolio portfolio;
        portfolio.addInstrument(
            std::make_unique<EuropeanOption>(OptionType::Call, 100.0, 1.0, "AAPL"),
            -1  // Short 1 call
        );
        
        std::map<std::string, MarketData> market_data_map;
        market_data_map["AAPL"] = createMarketData("AAPL", 100.0, 0.05, 0.2);
        
        RiskEngine engine;
        PortfolioRiskResult result = engine.calculatePortfolioRisk(portfolio, market_data_map);
        
        // Should be negative of long position
        suite.assert_equal(-10.4506, result.total_pv, 0.01, "Negative PV");
        suite.assert_equal(-0.6368, result.total_delta, 0.01, "Negative Delta");
        suite.assert_equal(-0.0188, result.total_gamma, 0.001, "Negative Gamma");
    });
}

void test_portfolio_aggregation(TestSuite& suite) {
    suite.run_test("Multiple instruments aggregate correctly", [&]() {
        Portfolio portfolio;
        
        // Long 2 calls
        portfolio.addInstrument(
            std::make_unique<EuropeanOption>(OptionType::Call, 100.0, 1.0, "AAPL"),
            2
        );
        
        // Long 3 puts
        portfolio.addInstrument(
            std::make_unique<EuropeanOption>(OptionType::Put, 100.0, 1.0, "AAPL"),
            3
        );
        
        std::map<std::string, MarketData> market_data_map;
        market_data_map["AAPL"] = createMarketData("AAPL", 100.0, 0.05, 0.2);
        
        RiskEngine engine;
        PortfolioRiskResult result = engine.calculatePortfolioRisk(portfolio, market_data_map);
        
        // Total PV = 2*10.4506 + 3*5.5735 = 20.9012 + 16.7205 = 37.6217
        suite.assert_equal(37.6217, result.total_pv, 0.01, "Aggregated PV");
        
        // Total Delta = 2*0.6368 + 3*(-0.3632) = 1.2736 - 1.0896 = 0.1840
        suite.assert_equal(0.1840, result.total_delta, 0.01, "Aggregated Delta");
        
        // Total Gamma = 2*0.0188 + 3*0.0188 = 0.0940
        suite.assert_equal(0.0940, result.total_gamma, 0.001, "Aggregated Gamma");
        
        // Total Vega = 2*37.5245 + 3*37.5245 = 187.6225
        suite.assert_equal(187.6225, result.total_vega, 1.0, "Aggregated Vega");
    });
}

void test_delta_neutral_portfolio(TestSuite& suite) {
    suite.run_test("Delta neutral portfolio", [&]() {
        Portfolio portfolio;
        
        // Create a delta-neutral portfolio
        // Call delta ≈ 0.6368, Put delta ≈ -0.3632
        // With 7 calls: delta = 7 × 0.6368 = 4.4576
        // To neutralize: need 4.4576 / 0.3632 ≈ 12.27 puts (LONG, not short)
        portfolio.addInstrument(
            std::make_unique<EuropeanOption>(OptionType::Call, 100.0, 1.0, "AAPL"),
            7  // 7 long calls
        );
        
        portfolio.addInstrument(
            std::make_unique<EuropeanOption>(OptionType::Put, 100.0, 1.0, "AAPL"),
            12  // 12 LONG puts (positive quantity)
        );
        
        std::map<std::string, MarketData> market_data_map;
        market_data_map["AAPL"] = createMarketData("AAPL", 100.0, 0.05, 0.2);
        
        RiskEngine engine;
        PortfolioRiskResult result = engine.calculatePortfolioRisk(portfolio, market_data_map);
        
        // Delta should be close to zero (within 0.5 due to rounding)
        suite.assert_equal(0.0, result.total_delta, 0.5, "Near zero delta");
        
        // Gamma should still be positive (long gamma from both long calls and long puts)
        if (result.total_gamma <= 0.0) {
            throw std::runtime_error("Delta neutral portfolio should have positive gamma");
        }
    });
}

void test_multi_asset_portfolio(TestSuite& suite) {
    suite.run_test("Portfolio with multiple underlying assets", [&]() {
        Portfolio portfolio;
        
        portfolio.addInstrument(
            std::make_unique<EuropeanOption>(OptionType::Call, 100.0, 1.0, "AAPL"),
            1
        );
        
        portfolio.addInstrument(
            std::make_unique<EuropeanOption>(OptionType::Put, 150.0, 0.5, "GOOGL"),
            2
        );
        
        std::map<std::string, MarketData> market_data_map;
        market_data_map["AAPL"] = createMarketData("AAPL", 100.0, 0.05, 0.2);
        market_data_map["GOOGL"] = createMarketData("GOOGL", 150.0, 0.05, 0.25);
        
        RiskEngine engine;
        PortfolioRiskResult result = engine.calculatePortfolioRisk(portfolio, market_data_map);
        
        // Should aggregate across both assets
        if (result.total_pv <= 0.0) {
            throw std::runtime_error("Multi-asset portfolio should have positive PV");
        }
        
        // Greeks should be non-zero
        if (result.total_gamma <= 0.0) {
            throw std::runtime_error("Multi-asset portfolio should have positive gamma");
        }
    });
}

void test_var_properties(TestSuite& suite) {
    suite.run_test("VaR increases with position size", [&]() {
        // Small position
        Portfolio small_portfolio;
        small_portfolio.addInstrument(
            std::make_unique<EuropeanOption>(OptionType::Call, 100.0, 1.0, "AAPL"),
            1
        );
        
        // Large position
        Portfolio large_portfolio;
        large_portfolio.addInstrument(
            std::make_unique<EuropeanOption>(OptionType::Call, 100.0, 1.0, "AAPL"),
            10
        );
        
        std::map<std::string, MarketData> market_data_map;
        market_data_map["AAPL"] = createMarketData("AAPL", 100.0, 0.05, 0.2);
        
        RiskEngine engine;
        PortfolioRiskResult small_result = engine.calculatePortfolioRisk(small_portfolio, market_data_map);
        PortfolioRiskResult large_result = engine.calculatePortfolioRisk(large_portfolio, market_data_map);
        
        // Larger position should have larger VaR
        if (large_result.value_at_risk_95 <= small_result.value_at_risk_95) {
            throw std::runtime_error("VaR should increase with position size");
        }
        
        // VaR should scale roughly linearly (within Monte Carlo noise)
        double var_ratio = large_result.value_at_risk_95 / small_result.value_at_risk_95;
        if (var_ratio < 8.0 || var_ratio > 12.0) {
            throw std::runtime_error("VaR scaling seems off: ratio = " + std::to_string(var_ratio));
        }
    });
    
    suite.run_test("VaR is non-negative", [&]() {
        Portfolio portfolio;
        portfolio.addInstrument(
            std::make_unique<EuropeanOption>(OptionType::Call, 100.0, 1.0, "AAPL"),
            1
        );
        
        std::map<std::string, MarketData> market_data_map;
        market_data_map["AAPL"] = createMarketData("AAPL", 100.0, 0.05, 0.2);
        
        RiskEngine engine;
        PortfolioRiskResult result = engine.calculatePortfolioRisk(portfolio, market_data_map);
        
        if (result.value_at_risk_95 < 0.0) {
            throw std::runtime_error("VaR should be non-negative");
        }
    });
}

void test_theta_time_decay(TestSuite& suite) {
    suite.run_test("Theta is negative for long options", [&]() {
        Portfolio portfolio;
        portfolio.addInstrument(
            std::make_unique<EuropeanOption>(OptionType::Call, 100.0, 1.0, "AAPL"),
            1
        );
        
        std::map<std::string, MarketData> market_data_map;
        market_data_map["AAPL"] = createMarketData("AAPL", 100.0, 0.05, 0.2);
        
        RiskEngine engine;
        PortfolioRiskResult result = engine.calculatePortfolioRisk(portfolio, market_data_map);
        
        // Long options should have negative theta (time decay)
        if (result.total_theta >= 0.0) {
            throw std::runtime_error("Long option should have negative theta");
        }
    });
}

int main() {
    TestSuite suite;
    
    std::cout << "\n" << std::string(60, '=') << std::endl;
    std::cout << "  RiskEngine Test Suite" << std::endl;
    std::cout << std::string(60, '=') << "\n" << std::endl;
    
    test_empty_portfolio(suite);
    test_single_call_option(suite);
    test_single_put_option(suite);
    test_quantity_scaling(suite);
    test_negative_quantity(suite);
    test_portfolio_aggregation(suite);
    test_delta_neutral_portfolio(suite);
    test_multi_asset_portfolio(suite);
    test_var_properties(suite);
    test_theta_time_decay(suite);
    
    suite.print_summary();
    
    return suite.all_passed() ? 0 : 1;
}
