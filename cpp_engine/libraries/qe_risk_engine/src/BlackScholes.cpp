#include "BlackScholes.h"
#include <cmath>
#include <algorithm>
#include <limits>

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

namespace BlackScholes {

double N(double z) {
    return 0.5 * (1.0 + std::erf(z / std::sqrt(2.0)));
}

double nPrime(double z) {
    return (1.0 / std::sqrt(2.0 * M_PI)) * std::exp(-0.5 * z * z);
}

void validateInputs(double S, double K, double r, double T, double sigma) {
    if (S <= 0.0) {
        throw std::invalid_argument("Spot price must be positive");
    }
    if (K <= 0.0) {
        throw std::invalid_argument("Strike price must be positive");
    }
    if (T < 0.0) {
        throw std::invalid_argument("Time to expiry cannot be negative");
    }
    if (sigma < 0.0) {
        throw std::invalid_argument("Volatility cannot be negative");
    }
    if (std::isnan(S) || std::isinf(S)) {
        throw std::invalid_argument("Invalid spot price");
    }
    if (std::isnan(K) || std::isinf(K)) {
        throw std::invalid_argument("Invalid strike price");
    }
    if (std::isnan(r) || std::isinf(r)) {
        throw std::invalid_argument("Invalid risk-free rate");
    }
    if (std::isnan(T) || std::isinf(T)) {
        throw std::invalid_argument("Invalid time to expiry");
    }
    if (std::isnan(sigma) || std::isinf(sigma)) {
        throw std::invalid_argument("Invalid volatility");
    }
}

double callPrice(double S, double K, double r, double T, double sigma) {
    validateInputs(S, K, r, T, sigma);
    
    if (T <= 0.0 || sigma <= 0.0) {
        return std::max(0.0, S - K);
    }
    
    const double d1 = (std::log(S / K) + (r + 0.5 * sigma * sigma) * T) / (sigma * std::sqrt(T));
    const double d2 = d1 - sigma * std::sqrt(T);
    
    return S * N(d1) - K * std::exp(-r * T) * N(d2);
}

double putPrice(double S, double K, double r, double T, double sigma) {
    validateInputs(S, K, r, T, sigma);
    
    if (T <= 0.0 || sigma <= 0.0) {
        return std::max(0.0, K - S);
    }
    
    const double d1 = (std::log(S / K) + (r + 0.5 * sigma * sigma) * T) / (sigma * std::sqrt(T));
    const double d2 = d1 - sigma * std::sqrt(T);
    
    return K * std::exp(-r * T) * N(-d2) - S * N(-d1);
}

double callDelta(double S, double K, double r, double T, double sigma) {
    validateInputs(S, K, r, T, sigma);
    
    if (T <= 0.0 || sigma <= 0.0) {
        return (S > K) ? 1.0 : 0.0;
    }
    
    const double d1 = (std::log(S / K) + (r + 0.5 * sigma * sigma) * T) / (sigma * std::sqrt(T));
    return N(d1);
}

double putDelta(double S, double K, double r, double T, double sigma) {
    validateInputs(S, K, r, T, sigma);
    
    if (T <= 0.0 || sigma <= 0.0) {
        return (S < K) ? -1.0 : 0.0;
    }
    
    const double d1 = (std::log(S / K) + (r + 0.5 * sigma * sigma) * T) / (sigma * std::sqrt(T));
    return N(d1) - 1.0;
}

double gamma(double S, double K, double r, double T, double sigma) {
    validateInputs(S, K, r, T, sigma);
    
    if (T <= 0.0 || sigma <= 0.0) {
        return 0.0;
    }
    
    const double d1 = (std::log(S / K) + (r + 0.5 * sigma * sigma) * T) / (sigma * std::sqrt(T));
    return nPrime(d1) / (S * sigma * std::sqrt(T));
}

double vega(double S, double K, double r, double T, double sigma) {
    validateInputs(S, K, r, T, sigma);
    
    if (T <= 0.0 || sigma <= 0.0) {
        return 0.0;
    }
    
    const double d1 = (std::log(S / K) + (r + 0.5 * sigma * sigma) * T) / (sigma * std::sqrt(T));
    return S * nPrime(d1) * std::sqrt(T);
}

double callTheta(double S, double K, double r, double T, double sigma) {
    validateInputs(S, K, r, T, sigma);
    
    if (T <= 0.0 || sigma <= 0.0) {
        return 0.0;
    }
    
    const double d1 = (std::log(S / K) + (r + 0.5 * sigma * sigma) * T) / (sigma * std::sqrt(T));
    const double d2 = d1 - sigma * std::sqrt(T);
    
    const double term1 = -(S * nPrime(d1) * sigma) / (2.0 * std::sqrt(T));
    const double term2 = r * K * std::exp(-r * T) * N(d2);
    
    return (term1 - term2) / 365.0;
}

double putTheta(double S, double K, double r, double T, double sigma) {
    validateInputs(S, K, r, T, sigma);
    
    if (T <= 0.0 || sigma <= 0.0) {
        return 0.0;
    }
    
    const double d1 = (std::log(S / K) + (r + 0.5 * sigma * sigma) * T) / (sigma * std::sqrt(T));
    const double d2 = d1 - sigma * std::sqrt(T);
    
    const double term1 = -(S * nPrime(d1) * sigma) / (2.0 * std::sqrt(T));
    const double term2 = r * K * std::exp(-r * T) * N(-d2);
    
    return (term1 + term2) / 365.0;
}

double callRho(double S, double K, double r, double T, double sigma) {
    validateInputs(S, K, r, T, sigma);
    
    if (T <= 0.0) {
        return 0.0;
    }
    
    const double d1 = (std::log(S / K) + (r + 0.5 * sigma * sigma) * T) / (sigma * std::sqrt(T));
    const double d2 = d1 - sigma * std::sqrt(T);
    
    return K * T * std::exp(-r * T) * N(d2) / 100.0;
}

double putRho(double S, double K, double r, double T, double sigma) {
    validateInputs(S, K, r, T, sigma);
    
    if (T <= 0.0) {
        return 0.0;
    }
    
    const double d1 = (std::log(S / K) + (r + 0.5 * sigma * sigma) * T) / (sigma * std::sqrt(T));
    const double d2 = d1 - sigma * std::sqrt(T);
    
    return -K * T * std::exp(-r * T) * N(-d2) / 100.0;
}

double impliedVolatility(
    double market_price, double S, double K, double r, double T,
    bool is_call, double initial_guess, double tolerance,
    int max_iterations
) {
    validateInputs(S, K, r, T, initial_guess);
    
    if (market_price < 0.0) {
        throw std::invalid_argument("Market price cannot be negative");
    }
    
    if (T <= 0.0) {
        throw std::invalid_argument("Cannot calculate implied volatility for expired option");
    }
    
    const double intrinsic = is_call ? std::max(0.0, S - K) : std::max(0.0, K - S);
    if (market_price < intrinsic - 1e-10) {
        throw std::invalid_argument("Market price below intrinsic value");
    }
    
    double sigma = initial_guess;
    
    for (int i = 0; i < max_iterations; ++i) {
        double price;
        try {
            price = is_call ? callPrice(S, K, r, T, sigma) : putPrice(S, K, r, T, sigma);
        } catch (...) {
            throw std::runtime_error("Failed to calculate option price during IV search");
        }
        
        const double price_diff = price - market_price;
        
        if (std::abs(price_diff) < tolerance) {
            return sigma;
        }
        
        const double vega_val = vega(S, K, r, T, sigma);
        
        if (vega_val < 1e-10) {
            throw std::runtime_error("Vega too small for Newton-Raphson");
        }
        
        sigma = sigma - price_diff / vega_val;
        
        if (sigma <= 0.0) {
            sigma = 0.01;
        }
        if (sigma > 10.0) {
            sigma = 10.0;
        }
    }
    
    throw std::runtime_error("Implied volatility did not converge");
}

}