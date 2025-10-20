#include "simple_test.h"
#include "../src/utils/BlackScholes.h"
#include <cmath>

void test_cumulative_normal(TestSuite& suite) {
    suite.run_test("N(0) should be 0.5", [&]() {
        suite.assert_equal(0.5, BlackScholes::N(0.0), 1e-10);
    });
    
    suite.run_test("N is symmetric around 0", [&]() {
        double z = 1.5;
        double nz = BlackScholes::N(z);
        double n_neg_z = BlackScholes::N(-z);
        suite.assert_equal(1.0, nz + n_neg_z, 1e-10, "N(z) + N(-z) = 1");
    });
}

void test_call_price(TestSuite& suite) {
    suite.run_test("Call price - ATM option", [&]() {
        double price = BlackScholes::callPrice(100.0, 100.0, 0.05, 1.0, 0.2);
        suite.assert_equal(10.4506, price, 0.01);
    });
    
    suite.run_test("Call price - ITM option", [&]() {
        double price = BlackScholes::callPrice(110.0, 100.0, 0.05, 1.0, 0.2);
        suite.assert_equal(17.6630, price, 0.01);
    });
    
    suite.run_test("Call price - OTM option", [&]() {
        double price = BlackScholes::callPrice(90.0, 100.0, 0.05, 1.0, 0.2);
        suite.assert_equal(5.0912, price, 0.01);
    });
    
    suite.run_test("Call intrinsic value when T=0", [&]() {
        double price = BlackScholes::callPrice(110.0, 100.0, 0.05, 0.0, 0.2);
        suite.assert_equal(10.0, price, 1e-10, "Should return max(S-K, 0)");
    });
    
    suite.run_test("Call zero when OTM and T=0", [&]() {
        double price = BlackScholes::callPrice(90.0, 100.0, 0.05, 0.0, 0.2);
        suite.assert_equal(0.0, price, 1e-10);
    });
}

void test_put_price(TestSuite& suite) {
    suite.run_test("Put price - ATM option", [&]() {
        double price = BlackScholes::putPrice(100.0, 100.0, 0.05, 1.0, 0.2);
        suite.assert_equal(5.5735, price, 0.01);
    });
    
    suite.run_test("Put intrinsic value when T=0", [&]() {
        double price = BlackScholes::putPrice(90.0, 100.0, 0.05, 0.0, 0.2);
        suite.assert_equal(10.0, price, 1e-10, "Should return max(K-S, 0)");
    });
}

void test_put_call_parity(TestSuite& suite) {
    suite.run_test("Put-Call parity - ATM", [&]() {
        double S = 100.0, K = 100.0, r = 0.05, T = 1.0, sigma = 0.2;
        double call = BlackScholes::callPrice(S, K, r, T, sigma);
        double put = BlackScholes::putPrice(S, K, r, T, sigma);
        double lhs = call - put;
        double rhs = S - K * std::exp(-r * T);
        suite.assert_equal(rhs, lhs, 1e-8);
    });
    
    suite.run_test("Put-Call parity - ITM", [&]() {
        double S = 110.0, K = 100.0, r = 0.05, T = 1.0, sigma = 0.2;
        double call = BlackScholes::callPrice(S, K, r, T, sigma);
        double put = BlackScholes::putPrice(S, K, r, T, sigma);
        double lhs = call - put;
        double rhs = S - K * std::exp(-r * T);
        suite.assert_equal(rhs, lhs, 1e-8);
    });
}

void test_delta(TestSuite& suite) {
    suite.run_test("Call delta - ATM", [&]() {
        double delta = BlackScholes::callDelta(100.0, 100.0, 0.05, 1.0, 0.2);
        suite.assert_equal(0.6368, delta, 0.01);
        if (delta < 0.0 || delta > 1.0) {
            throw std::runtime_error("Call delta out of range [0,1]");
        }
    });
    
    suite.run_test("Put delta - ATM", [&]() {
        double delta = BlackScholes::putDelta(100.0, 100.0, 0.05, 1.0, 0.2);
        suite.assert_equal(-0.3632, delta, 0.01);
        if (delta < -1.0 || delta > 0.0) {
            throw std::runtime_error("Put delta out of range [-1,0]");
        }
    });
    
    suite.run_test("Delta relationship: put = call - 1", [&]() {
        double S = 100.0, K = 100.0, r = 0.05, T = 1.0, sigma = 0.2;
        double callD = BlackScholes::callDelta(S, K, r, T, sigma);
        double putD = BlackScholes::putDelta(S, K, r, T, sigma);
        suite.assert_equal(callD - 1.0, putD, 1e-10);
    });
    
    suite.run_test("Call delta when T=0 and ITM", [&]() {
        double delta = BlackScholes::callDelta(110.0, 100.0, 0.05, 0.0, 0.2);
        suite.assert_equal(1.0, delta, 1e-10);
    });
    
    suite.run_test("Put delta when T=0 and ITM", [&]() {
        double delta = BlackScholes::putDelta(90.0, 100.0, 0.05, 0.0, 0.2);
        suite.assert_equal(-1.0, delta, 1e-10);
    });
}

void test_gamma(TestSuite& suite) {
    suite.run_test("Gamma - ATM", [&]() {
        double g = BlackScholes::gamma(100.0, 100.0, 0.05, 1.0, 0.2);
        suite.assert_equal(0.0188, g, 0.001);
        if (g < 0.0) {
            throw std::runtime_error("Gamma should be non-negative");
        }
    });
    
    suite.run_test("Gamma when T=0", [&]() {
        double g = BlackScholes::gamma(100.0, 100.0, 0.05, 0.0, 0.2);
        suite.assert_equal(0.0, g, 1e-10);
    });
    
    suite.run_test("Gamma peaks at ATM", [&]() {
        double gamma_atm = BlackScholes::gamma(100.0, 100.0, 0.05, 1.0, 0.2);
        double gamma_itm = BlackScholes::gamma(120.0, 100.0, 0.05, 1.0, 0.2);
        double gamma_otm = BlackScholes::gamma(80.0, 100.0, 0.05, 1.0, 0.2);
        
        if (gamma_atm <= gamma_itm || gamma_atm <= gamma_otm) {
            throw std::runtime_error("Gamma should peak at ATM");
        }
    });
}

void test_vega(TestSuite& suite) {
    suite.run_test("Vega - ATM", [&]() {
        double v = BlackScholes::vega(100.0, 100.0, 0.05, 1.0, 0.2);
        suite.assert_equal(37.5245, v, 0.1);
        if (v < 0.0) {
            throw std::runtime_error("Vega should be non-negative");
        }
    });
    
    suite.run_test("Vega when T=0", [&]() {
        double v = BlackScholes::vega(100.0, 100.0, 0.05, 0.0, 0.2);
        suite.assert_equal(0.0, v, 1e-10);
    });
}

void test_theta(TestSuite& suite) {
    suite.run_test("Call theta - ATM", [&]() {
        double theta = BlackScholes::callTheta(100.0, 100.0, 0.05, 1.0, 0.2);
        // Theta is typically negative for long options (time decay)
        if (theta > 0.0) {
            throw std::runtime_error("Call theta should typically be negative");
        }
        suite.assert_equal(-0.0178, theta, 0.001);
    });
    
    suite.run_test("Put theta - ATM", [&]() {
        double theta = BlackScholes::putTheta(100.0, 100.0, 0.05, 1.0, 0.2);
        suite.assert_equal(-0.0042, theta, 0.001);
    });
    
    suite.run_test("Theta when T=0", [&]() {
        double theta = BlackScholes::callTheta(100.0, 100.0, 0.05, 0.0, 0.2);
        suite.assert_equal(0.0, theta, 1e-10);
    });
}

int main() {
    TestSuite suite;
    
    std::cout << "\n" << std::string(60, '=') << std::endl;
    std::cout << "  BlackScholes Module Test Suite" << std::endl;
    std::cout << std::string(60, '=') << "\n" << std::endl;
    
    test_cumulative_normal(suite);
    test_call_price(suite);
    test_put_price(suite);
    test_put_call_parity(suite);
    test_delta(suite);
    test_gamma(suite);
    test_vega(suite);
    test_theta(suite);
    
    suite.print_summary();
    
    return suite.all_passed() ? 0 : 1;
}
