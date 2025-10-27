#ifndef BINOMIALTREE_H
#define BINOMIALTREE_H

#include "Instrument.h"
#include <vector>

#ifdef USE_QUANTLIB
#include "QuantLibPricingEngine.h"
#endif

namespace BinomialTree {
double europeanOptionPrice(double S, double K, double r, double T, double sigma,
                           OptionType type, int steps);

double americanOptionPrice(double S, double K, double r, double T, double sigma,
                           OptionType type, int steps);

struct TreeNode {
  double stock_price;
  double option_value;
  bool exercise_optimal;
};

std::vector<std::vector<TreeNode>> buildTree(double S, double K, double r,
                                             double T, double sigma,
                                             OptionType type, int steps,
                                             bool is_american);

#ifdef USE_QUANTLIB
// Validation functions that compare with QuantLib
QuantLibPricer::ValidationResult validateEuropeanPrice(
    double S, double K, double r, double T, double sigma,
    OptionType type, int steps, double tolerance = 1e-4
);

QuantLibPricer::ValidationResult validateAmericanPrice(
    double S, double K, double r, double T, double sigma,
    OptionType type, int steps, double tolerance = 1e-4
);
#endif

} // namespace BinomialTree

#endif