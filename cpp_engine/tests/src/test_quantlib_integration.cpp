#ifdef USE_QUANTLIB

#include "BlackScholes.h"
#include "BinomialTree.h"
#include "Instrument.h"
#include "QuantLibPricingEngine.h"
#include "simple_test.h"
#include <iostream>
#include <iomanip>

const double REGRESSION_TOLERANCE = 1e-5;
const double BINOMIAL_TOLERANCE = 1e-4;

void test_blackscholes_validation(TestSuite &suite) {
    std::cout << "\n=== Black-Scholes QuantLib Validation ===" << std::endl;
    
    suite.run_test("BS Validation: ATM Call", [&]() {
        auto result = BlackScholes::validateCallPrice(
            100.0, 100.0, 0.05, 1.0, 0.2, REGRESSION_TOLERANCE
        );
        std::cout << "  " << result.message << std::endl;
        suite.assert_equal(1.0, result.within_tolerance ? 1.0 : 0.0, 0.1, 
            "ATM call validation failed");
    });

    suite.run_test("BS Validation: ITM Call", [&]() {
        auto result = BlackScholes::validateCallPrice(
            110.0, 100.0, 0.05, 1.0, 0.2, REGRESSION_TOLERANCE
        );
        std::cout << "  " << result.message << std::endl;
        suite.assert_equal(1.0, result.within_tolerance ? 1.0 : 0.0, 0.1,
            "ITM call validation failed");
    });

    suite.run_test("BS Validation: OTM Call", [&]() {
        auto result = BlackScholes::validateCallPrice(
            90.0, 100.0, 0.05, 1.0, 0.2, REGRESSION_TOLERANCE
        );
        std::cout << "  " << result.message << std::endl;
        suite.assert_equal(1.0, result.within_tolerance ? 1.0 : 0.0, 0.1,
            "OTM call validation failed");
    });

    suite.run_test("BS Validation: ATM Put", [&]() {
        auto result = BlackScholes::validatePutPrice(
            100.0, 100.0, 0.05, 1.0, 0.2, REGRESSION_TOLERANCE
        );
        std::cout << "  " << result.message << std::endl;
        suite.assert_equal(1.0, result.within_tolerance ? 1.0 : 0.0, 0.1,
            "ATM put validation failed");
    });
}

void test_binomial_validation(TestSuite &suite) {
    std::cout << "\n=== Binomial Tree QuantLib Validation ===" << std::endl;
    
    suite.run_test("Binomial Validation: European Call", [&]() {
        auto result = BinomialTree::validateEuropeanPrice(
            100.0, 100.0, 0.05, 1.0, 0.2, OptionType::Call, 100, BINOMIAL_TOLERANCE
        );
        std::cout << "  " << result.message << std::endl;
        suite.assert_equal(1.0, result.within_tolerance ? 1.0 : 0.0, 0.1,
            "European call binomial validation failed");
    });

    suite.run_test("Binomial Validation: American Put", [&]() {
        auto result = BinomialTree::validateAmericanPrice(
            100.0, 100.0, 0.05, 1.0, 0.2, OptionType::Put, 100, BINOMIAL_TOLERANCE
        );
        std::cout << "  " << result.message << std::endl;
        suite.assert_equal(1.0, result.within_tolerance ? 1.0 : 0.0, 0.1,
            "American put binomial validation failed");
    });
}

void test_exotic_options(TestSuite &suite) {
    std::cout << "\n=== Exotic Options Pricing ===" << std::endl;
    
    MarketData md{"TEST", 100.0, 0.05, 0.2};
    
    suite.run_test("Barrier Option: Down-and-Out Call", [&]() {
        BarrierOption barrier(OptionType::Call, 100.0, 90.0, 
                             BarrierOption::BarrierType::DownOut, 1.0, "TEST");
        double price = barrier.price(md);
        std::cout << "  Down-and-Out Call price: " << price << std::endl;
        suite.assert_equal(1.0, (price > 0 && price < 15) ? 1.0 : 0.0, 0.1,
            "Barrier option price out of expected range");
    });

    suite.run_test("Asian Option: Arithmetic Average Call", [&]() {
        AsianOption asian(OptionType::Call, 100.0, 1.0, "TEST",
                         AsianOption::AverageType::Arithmetic, 12);
        double price = asian.price(md);
        std::cout << "  Arithmetic Asian Call price: " << price << std::endl;
        suite.assert_equal(1.0, (price > 0 && price < 15) ? 1.0 : 0.0, 0.1,
            "Asian option price out of expected range");
    });
}

int main() {
    std::cout << "\n" << std::string(60, '=') << std::endl;
    std::cout << "  QuantLib Integration Test Suite" << std::endl;
    std::cout << std::string(60, '=') << std::endl;
    
    TestSuite suite;
    
    test_blackscholes_validation(suite);
    test_binomial_validation(suite);
    test_exotic_options(suite);
    
    std::cout << "\n" << std::string(60, '=') << std::endl;
    suite.print_summary();
    std::cout << std::string(60, '=') << std::endl;
    
    return suite.all_passed() ? 0 : 1;
}

#else

#include <iostream>

int main() {
    std::cout << "QuantLib tests skipped (USE_QUANTLIB not defined)" << std::endl;
    return 0;
}

#endif
