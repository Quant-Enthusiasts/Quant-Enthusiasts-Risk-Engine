#ifndef QUANTLIBPRICINGENGINE_H
#define QUANTLIBPRICINGENGINE_H

#ifdef USE_QUANTLIB

#include "Instrument.h"
#include "MarketData.h"
#include <string>

namespace QuantLibPricer {

/**
 * @brief Validation result comparing internal engine with QuantLib
 */
struct ValidationResult {
    double internal_price;
    double quantlib_price;
    double absolute_diff;
    double relative_diff;
    bool within_tolerance;
    std::string message;
    
    ValidationResult() 
        : internal_price(0.0), quantlib_price(0.0), 
          absolute_diff(0.0), relative_diff(0.0), 
          within_tolerance(false) {}
};

/**
 * @brief European option pricing using QuantLib Black-Scholes
 */
double europeanOptionBlackScholes(
    double S, double K, double r, double T, double sigma,
    OptionType type
);

/**
 * @brief European option pricing using QuantLib Binomial Tree
 */
double europeanOptionBinomial(
    double S, double K, double r, double T, double sigma,
    OptionType type, int steps = 100
);

/**
 * @brief American option pricing using QuantLib Binomial Tree
 */
double americanOptionBinomial(
    double S, double K, double r, double T, double sigma,
    OptionType type, int steps = 100
);

/**
 * @brief Validate internal Black-Scholes pricing against QuantLib
 */
ValidationResult validateBlackScholesPrice(
    double internal_price,
    double S, double K, double r, double T, double sigma,
    OptionType type,
    double tolerance = 1e-6
);

/**
 * @brief Validate internal Binomial pricing against QuantLib (European)
 */
ValidationResult validateBinomialPriceEuropean(
    double internal_price,
    double S, double K, double r, double T, double sigma,
    OptionType type, int steps = 100,
    double tolerance = 1e-4
);

/**
 * @brief Validate internal Binomial pricing against QuantLib (American)
 */
ValidationResult validateBinomialPriceAmerican(
    double internal_price,
    double S, double K, double r, double T, double sigma,
    OptionType type, int steps = 100,
    double tolerance = 1e-4
);

/**
 * @brief Greeks calculation using QuantLib
 */
struct GreeksResult {
    double delta;
    double gamma;
    double vega;
    double theta;
    double rho;
};

GreeksResult calculateGreeks(
    double S, double K, double r, double T, double sigma,
    OptionType type
);

// Exotic Options

/**
 * @brief Barrier option types
 */
enum class BarrierType {
    DownIn,    // Barrier below spot, option activates when breached
    DownOut,   // Barrier below spot, option deactivates when breached
    UpIn,      // Barrier above spot, option activates when breached
    UpOut      // Barrier above spot, option deactivates when breached
};

/**
 * @brief Price barrier option using QuantLib
 */
double barrierOptionPrice(
    double S, double K, double barrier, double r, double T, double sigma,
    OptionType option_type, BarrierType barrier_type,
    double rebate = 0.0
);

/**
 * @brief Asian option averaging types
 */
enum class AverageType {
    Arithmetic,
    Geometric
};

/**
 * @brief Price Asian option using QuantLib
 * @param past_fixings Historical price fixings (if any)
 * @param running_sum Running sum of past fixings for arithmetic average
 */
double asianOptionPrice(
    double S, double K, double r, double T, double sigma,
    OptionType option_type, AverageType average_type,
    int num_fixings, double running_sum = 0.0, int past_fixings = 0
);

} // namespace QuantLibPricer

#endif // USE_QUANTLIB

#endif // QUANTLIBPRICINGENGINE_H
