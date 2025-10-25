#include "BinomialTree.h"
#include <cmath>
#include <algorithm>
#include <stdexcept>

namespace BinomialTree {

double europeanOptionPrice(
    double S, double K, double r, double T, double sigma,
    OptionType type, int steps
) {
    if (S <= 0.0 || K <= 0.0) {
        throw std::invalid_argument("Stock price and strike must be positive");
    }
    if (T < 0.0) {
        throw std::invalid_argument("Time to expiry cannot be negative");
    }
    if (sigma < 0.0) {
        throw std::invalid_argument("Volatility cannot be negative");
    }
    if (steps < 1) {
        throw std::invalid_argument("Number of steps must be positive");
    }
    
    if (T == 0.0) {
        if (type == OptionType::Call) {
            return std::max(0.0, S - K);
        } else {
            return std::max(0.0, K - S);
        }
    }
    
    const double dt = T / steps;
    const double u = std::exp(sigma * std::sqrt(dt));
    const double d = 1.0 / u;
    const double p = (std::exp(r * dt) - d) / (u - d);
    const double discount = std::exp(-r * dt);
    
    if (p < 0.0 || p > 1.0) {
        throw std::runtime_error("Invalid probability in binomial tree");
    }
    
    std::vector<double> prices(steps + 1);
    
    for (int i = 0; i <= steps; ++i) {
        double spot_at_maturity = S * std::pow(u, steps - i) * std::pow(d, i);
        
        if (type == OptionType::Call) {
            prices[i] = std::max(0.0, spot_at_maturity - K);
        } else {
            prices[i] = std::max(0.0, K - spot_at_maturity);
        }
    }
    
    for (int step = steps - 1; step >= 0; --step) {
        for (int i = 0; i <= step; ++i) {
            prices[i] = discount * (p * prices[i] + (1.0 - p) * prices[i + 1]);
        }
    }
    
    return prices[0];
}

double americanOptionPrice(
    double S, double K, double r, double T, double sigma,
    OptionType type, int steps
) {
    if (S <= 0.0 || K <= 0.0) {
        throw std::invalid_argument("Stock price and strike must be positive");
    }
    if (T < 0.0) {
        throw std::invalid_argument("Time to expiry cannot be negative");
    }
    if (sigma < 0.0) {
        throw std::invalid_argument("Volatility cannot be negative");
    }
    if (steps < 1) {
        throw std::invalid_argument("Number of steps must be positive");
    }
    
    if (T == 0.0) {
        if (type == OptionType::Call) {
            return std::max(0.0, S - K);
        } else {
            return std::max(0.0, K - S);
        }
    }
    
    const double dt = T / steps;
    const double u = std::exp(sigma * std::sqrt(dt));
    const double d = 1.0 / u;
    const double p = (std::exp(r * dt) - d) / (u - d);
    const double discount = std::exp(-r * dt);
    
    if (p < 0.0 || p > 1.0) {
        throw std::runtime_error("Invalid probability in binomial tree");
    }
    
    std::vector<double> prices(steps + 1);
    std::vector<double> spots(steps + 1);
    
    for (int i = 0; i <= steps; ++i) {
        spots[i] = S * std::pow(u, steps - i) * std::pow(d, i);
        
        if (type == OptionType::Call) {
            prices[i] = std::max(0.0, spots[i] - K);
        } else {
            prices[i] = std::max(0.0, K - spots[i]);
        }
    }
    
    for (int step = steps - 1; step >= 0; --step) {
        for (int i = 0; i <= step; ++i) {
            spots[i] = S * std::pow(u, step - i) * std::pow(d, i);
            
            double hold_value = discount * (p * prices[i] + (1.0 - p) * prices[i + 1]);
            
            double exercise_value;
            if (type == OptionType::Call) {
                exercise_value = std::max(0.0, spots[i] - K);
            } else {
                exercise_value = std::max(0.0, K - spots[i]);
            }
            
            prices[i] = std::max(hold_value, exercise_value);
        }
    }
    
    return prices[0];
}

std::vector<std::vector<TreeNode>> buildTree(
    double S, double K, double r, double T, double sigma,
    OptionType type, int steps, bool is_american
) {
    std::vector<std::vector<TreeNode>> tree(steps + 1);
    
    const double dt = T / steps;
    const double u = std::exp(sigma * std::sqrt(dt));
    const double d = 1.0 / u;
    const double p = (std::exp(r * dt) - d) / (u - d);
    const double discount = std::exp(-r * dt);
    
    for (int step = 0; step <= steps; ++step) {
        tree[step].resize(step + 1);
        for (int i = 0; i <= step; ++i) {
            tree[step][i].stock_price = S * std::pow(u, step - i) * std::pow(d, i);
        }
    }
    
    for (int i = 0; i <= steps; ++i) {
        double spot = tree[steps][i].stock_price;
        if (type == OptionType::Call) {
            tree[steps][i].option_value = std::max(0.0, spot - K);
        } else {
            tree[steps][i].option_value = std::max(0.0, K - spot);
        }
        tree[steps][i].exercise_optimal = false;
    }
    
    for (int step = steps - 1; step >= 0; --step) {
        for (int i = 0; i <= step; ++i) {
            double hold_value = discount * (
                p * tree[step + 1][i].option_value + 
                (1.0 - p) * tree[step + 1][i + 1].option_value
            );
            
            double exercise_value = 0.0;
            if (type == OptionType::Call) {
                exercise_value = std::max(0.0, tree[step][i].stock_price - K);
                } else {
            exercise_value = std::max(0.0, K - tree[step][i].stock_price);
                }
                if (is_american && exercise_value > hold_value) {
            tree[step][i].option_value = exercise_value;
            tree[step][i].exercise_optimal = true;
        } else {
            tree[step][i].option_value = hold_value;
            tree[step][i].exercise_optimal = false;
        }
    }}

return tree;}
}