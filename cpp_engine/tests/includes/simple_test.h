#ifndef SIMPLE_TEST_H
#define SIMPLE_TEST_H

#include <iostream>
#include <string>
#include <cmath>
#include <vector>
#include <functional>

class TestSuite {
private:
    int passed = 0;
    int failed = 0;
    std::vector<std::string> failures;
    
public:
    void assert_equal(double expected, double actual, double epsilon = 1e-6, 
                      const std::string& msg = "") {
        if (std::abs(expected - actual) > epsilon) {
            std::string error = msg + " Expected: " + std::to_string(expected) + 
                              ", Got: " + std::to_string(actual) + 
                              ", Diff: " + std::to_string(std::abs(expected - actual));
            failures.push_back(error);
            throw std::runtime_error(error);
        }
    }
    
    void run_test(const std::string& name, std::function<void()> fn) {
        std::cout << "Running: " << name << " ... ";
        try {
            fn();
            passed++;
            std::cout << "✓ PASS" << std::endl;
        } catch (const std::exception& e) {
            failed++;
            std::cout << "✗ FAIL\n  " << e.what() << std::endl;
        }
    }
    
    void print_summary() {
        std::cout << "\n" << std::string(50, '=') << std::endl;
        std::cout << "Test Results: " << passed << " passed, " << failed << " failed" << std::endl;
        std::cout << std::string(50, '=') << std::endl;
    }
    
    bool all_passed() const { return failed == 0; }
};

#endif
