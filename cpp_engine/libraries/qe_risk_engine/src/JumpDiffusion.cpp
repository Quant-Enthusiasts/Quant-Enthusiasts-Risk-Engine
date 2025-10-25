#include "JumpDiffusion.h"
#include "BlackScholes.h"
#include <cmath>
#include <stdexcept>
#include <algorithm>

namespace JumpDiffusion {

double poissonProbability(int n, double lambda_t) {
    if (lambda_t < 0.0) {
        throw std::invalid_argument("Lambda * T must be non-negative");
    }
    if (n < 0) {
        throw std::invalid_argument("Number of jumps cannot be negative");
    }
    
    if (lambda_t == 0.0) {
        return n == 0 ? 1.0 : 0.0;
    }
    
    double log_prob = n * std::log(lambda_t) - lambda_t;
    for (int i = 2; i <= n; ++i) {
        log_prob -= std::log(static_cast<double>(i));
    }
    
    return std::exp(log_prob);
}

double mertonCallPrice(
    double S, double K, double r, double T, double sigma,
    double lambda, double jump_mean, double jump_vol,
    int max_jumps
) {
    if (S <= 0.0 || K <= 0.0) {
        throw std::invalid_argument("Stock price and strike must be positive");
    }
    if (T < 0.0) {
        throw std::invalid_argument("Time to expiry cannot be negative");
    }
    if (sigma < 0.0 || jump_vol < 0.0) {
        throw std::invalid_argument("Volatilities cannot be negative");
    }
    if (lambda < 0.0) {
        throw std::invalid_argument("Jump intensity must be non-negative");
    }
    
    if (T == 0.0) {
        return std::max(0.0, S - K);
    }
    
    const double k = std::exp(jump_mean + 0.5 * jump_vol * jump_vol) - 1.0;
    // const double lambda_prime = lambda * (1.0 + k);
    
    double option_value = 0.0;
    double sum_prob = 0.0;
    
    for (int n = 0; n <= max_jumps; ++n) {
        const double prob = poissonProbability(n, lambda * T);
        
        if (prob < 1e-10) {
            break;
        }
        
        sum_prob += prob;
        
        const double sigma_n = std::sqrt(
            sigma * sigma + n * jump_vol * jump_vol / T
        );
        
        const double r_n = r - lambda * k + n * (jump_mean + 0.5 * jump_vol * jump_vol) / T;
        
        const double bs_price = BlackScholes::callPrice(S, K, r_n, T, sigma_n);
        
        option_value += prob * bs_price;
        
        if (sum_prob > 0.9999 && prob < 1e-8) {
            break;
        }
    }
    
    if (std::isnan(option_value) || std::isinf(option_value)) {
        throw std::runtime_error("Invalid Merton jump diffusion price");
    }
    
    return option_value;
}

double mertonPutPrice(
    double S, double K, double r, double T, double sigma,
    double lambda, double jump_mean, double jump_vol,
    int max_jumps
) {
    if (S <= 0.0 || K <= 0.0) {
        throw std::invalid_argument("Stock price and strike must be positive");
    }
    if (T < 0.0) {
        throw std::invalid_argument("Time to expiry cannot be negative");
    }
    if (sigma < 0.0 || jump_vol < 0.0) {
        throw std::invalid_argument("Volatilities cannot be negative");
    }
    if (lambda < 0.0) {
        throw std::invalid_argument("Jump intensity must be non-negative");
    }
    
    if (T == 0.0) {
        return std::max(0.0, K - S);
    }
    
    const double k = std::exp(jump_mean + 0.5 * jump_vol * jump_vol) - 1.0;
    
    double option_value = 0.0;
    double sum_prob = 0.0;
    
    for (int n = 0; n <= max_jumps; ++n) {
        const double prob = poissonProbability(n, lambda * T);
        
        if (prob < 1e-10) {
            break;
        }
        
        sum_prob += prob;
        
        const double sigma_n = std::sqrt(
            sigma * sigma + n * jump_vol * jump_vol / T
        );
        
        const double r_n = r - lambda * k + n * (jump_mean + 0.5 * jump_vol * jump_vol) / T;
        
        const double bs_price = BlackScholes::putPrice(S, K, r_n, T, sigma_n);
        
        option_value += prob * bs_price;
        
        if (sum_prob > 0.9999 && prob < 1e-8) {
            break;
        }
    }
    
    if (std::isnan(option_value) || std::isinf(option_value)) {
        throw std::runtime_error("Invalid Merton jump diffusion price");
    }
    
    return option_value;
}

double mertonOptionPrice(
    double S, double K, double r, double T, double sigma,
    OptionType type, double lambda, double jump_mean, double jump_vol,
    int max_jumps
) {
    if (type == OptionType::Call) {
        return mertonCallPrice(S, K, r, T, sigma, lambda, jump_mean, jump_vol, max_jumps);
    } else {
        return mertonPutPrice(S, K, r, T, sigma, lambda, jump_mean, jump_vol, max_jumps);
    }
}

}
