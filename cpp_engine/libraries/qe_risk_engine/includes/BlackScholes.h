#ifndef BLACKSCHOLES_H
#define BLACKSCHOLES_H

#include <stdexcept>

namespace BlackScholes {
    double N(double z);
    double nPrime(double z);
    
    double callPrice(double S, double K, double r, double T, double sigma);
    double putPrice(double S, double K, double r, double T, double sigma);
    
    double callDelta(double S, double K, double r, double T, double sigma);
    double putDelta(double S, double K, double r, double T, double sigma);
    
    double gamma(double S, double K, double r, double T, double sigma);
    double vega(double S, double K, double r, double T, double sigma);
    
    double callTheta(double S, double K, double r, double T, double sigma);
    double putTheta(double S, double K, double r, double T, double sigma);
    
    double callRho(double S, double K, double r, double T, double sigma);
    double putRho(double S, double K, double r, double T, double sigma);
    
    double impliedVolatility(
        double market_price, double S, double K, double r, double T,
        bool is_call, double initial_guess = 0.3, double tolerance = 1e-6,
        int max_iterations = 100
    );
    
    void validateInputs(double S, double K, double r, double T, double sigma);
}

#endif