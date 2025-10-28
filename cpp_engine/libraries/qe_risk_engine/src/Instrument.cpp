#include "Instrument.h"
#include "BinomialTree.h"
#include "BlackScholes.h"
#include "JumpDiffusion.h"
#include <algorithm>
#include <cmath>
#include <limits>


EuropeanOption::EuropeanOption(OptionType type, double strike,
                               double time_to_expiry, std::string asset_id)
    : option_type_(type), strike_price_(strike),
      time_to_expiry_years_(time_to_expiry), underlying_asset_id_(asset_id),
      pricing_model_(PricingModel::BlackScholes), binomial_steps_(100),
      jump_intensity_(0.0), jump_mean_(0.0), jump_volatility_(0.0) {
  validateParameters();
}

EuropeanOption::EuropeanOption(OptionType type, double strike,
                               double time_to_expiry, std::string asset_id,
                               PricingModel model)
    : option_type_(type), strike_price_(strike),
      time_to_expiry_years_(time_to_expiry), underlying_asset_id_(asset_id),
      pricing_model_(model), binomial_steps_(100), jump_intensity_(0.0),
      jump_mean_(0.0), jump_volatility_(0.0) {
  validateParameters();
}

void EuropeanOption::validateParameters() const {
  if (strike_price_ <= 0.0) {
    throw std::invalid_argument("Strike price must be positive");
  }
  if (time_to_expiry_years_ < 0.0) {
    throw std::invalid_argument("Time to expiry cannot be negative");
  }
  if (underlying_asset_id_.empty()) {
    throw std::invalid_argument("Asset ID cannot be empty");
  }
  if (binomial_steps_ < 1 || binomial_steps_ > 10000) {
    throw std::invalid_argument("Binomial steps must be between 1 and 10000");
  }
  if (jump_intensity_ < 0.0) {
    throw std::invalid_argument("Jump intensity cannot be negative");
  }
}

void EuropeanOption::validateMarketData(const MarketData &md) const {
  if (md.spot_price <= 0.0) {
    throw std::invalid_argument("Spot price must be positive");
  }
  if (md.volatility < 0.0) {
    throw std::invalid_argument("Volatility cannot be negative");
  }
  if (std::isnan(md.spot_price) || std::isinf(md.spot_price)) {
    throw std::invalid_argument("Invalid spot price");
  }
  if (std::isnan(md.risk_free_rate) || std::isinf(md.risk_free_rate)) {
    throw std::invalid_argument("Invalid risk-free rate");
  }
  if (std::isnan(md.volatility) || std::isinf(md.volatility)) {
    throw std::invalid_argument("Invalid volatility");
  }
}

bool EuropeanOption::isValid() const {
  try {
    validateParameters();
    return true;
  } catch (...) {
    return false;
  }
}

std::string EuropeanOption::getInstrumentType() const {
  return "EuropeanOption";
}

void EuropeanOption::setPricingModel(PricingModel model) {
  pricing_model_ = model;
}

PricingModel EuropeanOption::getPricingModel() const { return pricing_model_; }

void EuropeanOption::setBinomialSteps(int steps) {
  if (steps < 1 || steps > 10000) {
    throw std::invalid_argument("Binomial steps must be between 1 and 10000");
  }
  binomial_steps_ = steps;
}

int EuropeanOption::getBinomialSteps() const { return binomial_steps_; }

void EuropeanOption::setJumpParameters(double lambda, double jump_mean,
                                       double jump_vol) {
  if (lambda < 0.0) {
    throw std::invalid_argument("Jump intensity must be non-negative");
  }
  if (jump_vol < 0.0) {
    throw std::invalid_argument("Jump volatility must be non-negative");
  }
  jump_intensity_ = lambda;
  jump_mean_ = jump_mean;
  jump_volatility_ = jump_vol;
}

double EuropeanOption::getJumpIntensity() const { return jump_intensity_; }

OptionType EuropeanOption::getOptionType() const { return option_type_; }

double EuropeanOption::getStrike() const { return strike_price_; }

double EuropeanOption::getTimeToExpiry() const { return time_to_expiry_years_; }

double EuropeanOption::priceBlackScholes(const MarketData &md) const {
  if (option_type_ == OptionType::Call) {
    return BlackScholes::callPrice(md.spot_price, strike_price_,
                                   md.risk_free_rate, time_to_expiry_years_,
                                   md.volatility);
  } else {
    return BlackScholes::putPrice(md.spot_price, strike_price_,
                                  md.risk_free_rate, time_to_expiry_years_,
                                  md.volatility);
  }
}

double EuropeanOption::priceBinomial(const MarketData &md) const {
  return BinomialTree::europeanOptionPrice(
      md.spot_price, strike_price_, md.risk_free_rate, time_to_expiry_years_,
      md.volatility, option_type_, binomial_steps_);
}

double EuropeanOption::priceJumpDiffusion(const MarketData &md) const {
  return JumpDiffusion::mertonOptionPrice(
      md.spot_price, strike_price_, md.risk_free_rate, time_to_expiry_years_,
      md.volatility, option_type_, jump_intensity_, jump_mean_,
      jump_volatility_);
}

double EuropeanOption::price(const MarketData &md) const {
  validateMarketData(md);

  double result = 0.0;

  switch (pricing_model_) {
  case PricingModel::BlackScholes:
    result = priceBlackScholes(md);
    break;
  case PricingModel::Binomial:
    result = priceBinomial(md);
    break;
  case PricingModel::MertonJumpDiffusion:
    result = priceJumpDiffusion(md);
    break;
  default:
    throw std::runtime_error("Unknown pricing model");
  }

  if (std::isnan(result) || std::isinf(result) || result < 0.0) {
    throw std::runtime_error("Invalid option price calculated");
  }

  return result;
}

double EuropeanOption::deltaBlackScholes(const MarketData &md) const {
  if (option_type_ == OptionType::Call) {
    return BlackScholes::callDelta(md.spot_price, strike_price_,
                                   md.risk_free_rate, time_to_expiry_years_,
                                   md.volatility);
  } else {
    return BlackScholes::putDelta(md.spot_price, strike_price_,
                                  md.risk_free_rate, time_to_expiry_years_,
                                  md.volatility);
  }
}

double EuropeanOption::deltaNumerical(const MarketData &md) const {
  const double bump = md.spot_price * 0.01;

  MarketData md_up = md;
  MarketData md_down = md;
  md_up.spot_price = md.spot_price + bump;
  md_down.spot_price = md.spot_price - bump;

  double price_up = price(md_up);
  double price_down = price(md_down);

  return (price_up - price_down) / (2.0 * bump);
}

double EuropeanOption::delta(const MarketData &md) const {
  validateMarketData(md);

  double result = 0.0;

  if (pricing_model_ == PricingModel::BlackScholes) {
    result = deltaBlackScholes(md);
  } else {
    result = deltaNumerical(md);
  }

  if (std::isnan(result) || std::isinf(result)) {
    throw std::runtime_error("Invalid delta calculated");
  }

  return result;
}

double EuropeanOption::gamma(const MarketData &md) const {
  validateMarketData(md);

  double result = 0.0;

  if (pricing_model_ == PricingModel::BlackScholes) {
    result =
        BlackScholes::gamma(md.spot_price, strike_price_, md.risk_free_rate,
                            time_to_expiry_years_, md.volatility);
  } else {
    const double bump = md.spot_price * 0.01;

    MarketData md_up = md;
    MarketData md_down = md;
    md_up.spot_price = md.spot_price + bump;
    md_down.spot_price = md.spot_price - bump;

    double delta_up = delta(md_up);
    double delta_down = delta(md_down);

    result = (delta_up - delta_down) / (2.0 * bump);
  }

  if (std::isnan(result) || std::isinf(result) || result < 0.0) {
    throw std::runtime_error("Invalid gamma calculated");
  }

  return result;
}

double EuropeanOption::vega(const MarketData &md) const {
  validateMarketData(md);

  double result = 0.0;

  if (pricing_model_ == PricingModel::BlackScholes) {
    result = BlackScholes::vega(md.spot_price, strike_price_, md.risk_free_rate,
                                time_to_expiry_years_, md.volatility);
  } else {
    const double bump = 0.01;

    MarketData md_up = md;
    MarketData md_down = md;
    md_up.volatility = md.volatility + bump;
    md_down.volatility = std::max(0.0, md.volatility - bump);

    double price_up = price(md_up);
    double price_down = price(md_down);

    result = (price_up - price_down) / (2.0 * bump);
  }

  if (std::isnan(result) || std::isinf(result) || result < 0.0) {
    throw std::runtime_error("Invalid vega calculated");
  }

  return result;
}

double EuropeanOption::theta(const MarketData &md) const {
  validateMarketData(md);

  double result = 0.0;

  if (pricing_model_ == PricingModel::BlackScholes) {
    if (option_type_ == OptionType::Call) {
      result = BlackScholes::callTheta(md.spot_price, strike_price_,
                                       md.risk_free_rate, time_to_expiry_years_,
                                       md.volatility);
    } else {
      result = BlackScholes::putTheta(md.spot_price, strike_price_,
                                      md.risk_free_rate, time_to_expiry_years_,
                                      md.volatility);
    }
  } else {
    const double bump = 1.0 / 365.0;

    if (time_to_expiry_years_ < bump) {
      return 0.0;
    }

    double current_price = price(md);

    EuropeanOption temp_option = *this;
    temp_option.time_to_expiry_years_ =
        std::max(0.0, time_to_expiry_years_ - bump);
    double future_price = temp_option.price(md);

    result = (future_price - current_price) / bump;
  }

  if (std::isnan(result) || std::isinf(result)) {
    throw std::runtime_error("Invalid theta calculated");
  }

  return result;
}

std::string EuropeanOption::getAssetId() const { return underlying_asset_id_; }

AmericanOption::AmericanOption(OptionType type, double strike,
                               double time_to_expiry, std::string asset_id,
                               int binomial_steps)
    : option_type_(type), strike_price_(strike),
      time_to_expiry_years_(time_to_expiry), underlying_asset_id_(asset_id),
      binomial_steps_(binomial_steps) {
  validateParameters();
}

void AmericanOption::validateParameters() const {
  if (strike_price_ <= 0.0) {
    throw std::invalid_argument("Strike price must be positive");
  }
  if (time_to_expiry_years_ < 0.0) {
    throw std::invalid_argument("Time to expiry cannot be negative");
  }
  if (underlying_asset_id_.empty()) {
    throw std::invalid_argument("Asset ID cannot be empty");
  }
  if (binomial_steps_ < 1 || binomial_steps_ > 10000) {
    throw std::invalid_argument("Binomial steps must be between 1 and 10000");
  }
}

void AmericanOption::validateMarketData(const MarketData &md) const {
  if (md.spot_price <= 0.0) {
    throw std::invalid_argument("Spot price must be positive");
  }
  if (md.volatility < 0.0) {
    throw std::invalid_argument("Volatility cannot be negative");
  }
}

bool AmericanOption::isValid() const {
  try {
    validateParameters();
    return true;
  } catch (...) {
    return false;
  }
}

std::string AmericanOption::getInstrumentType() const {
  return "AmericanOption";
}

void AmericanOption::setBinomialSteps(int steps) {
  if (steps < 1 || steps > 10000) {
    throw std::invalid_argument("Binomial steps must be between 1 and 10000");
  }
  binomial_steps_ = steps;
}

int AmericanOption::getBinomialSteps() const { return binomial_steps_; }

double AmericanOption::calculateIntrinsicValue(double spot_price) const {
  if (option_type_ == OptionType::Call) {
    return std::max(0.0, spot_price - strike_price_);
  } else {
    return std::max(0.0, strike_price_ - spot_price);
  }
}

double AmericanOption::price(const MarketData &md) const {
  validateMarketData(md);

  double result = BinomialTree::americanOptionPrice(
      md.spot_price, strike_price_, md.risk_free_rate, time_to_expiry_years_,
      md.volatility, option_type_, binomial_steps_);

  if (std::isnan(result) || std::isinf(result) || result < 0.0) {
    throw std::runtime_error("Invalid American option price calculated");
  }

  return result;
}

double AmericanOption::delta(const MarketData &md) const {
  validateMarketData(md);

  const double bump = md.spot_price * 0.01;

  MarketData md_up = md;
  MarketData md_down = md;
  md_up.spot_price = md.spot_price + bump;
  md_down.spot_price = md.spot_price - bump;

  double price_up = price(md_up);
  double price_down = price(md_down);

  double result = (price_up - price_down) / (2.0 * bump);

  if (std::isnan(result) || std::isinf(result)) {
    throw std::runtime_error("Invalid delta calculated");
  }

  return result;
}

double AmericanOption::gamma(const MarketData &md) const {
  validateMarketData(md);

  const double bump = md.spot_price * 0.01;

  MarketData md_up = md;
  MarketData md_down = md;
  md_up.spot_price = md.spot_price + bump;
  md_down.spot_price = md.spot_price - bump;

  double delta_up = delta(md_up);
  double delta_down = delta(md_down);

  double result = (delta_up - delta_down) / (2.0 * bump);

  if (std::isnan(result) || std::isinf(result)) {
    throw std::runtime_error("Invalid gamma calculated");
  }

  return result;
}

double AmericanOption::vega(const MarketData &md) const {
  validateMarketData(md);

  const double bump = 0.01;

  MarketData md_up = md;
  MarketData md_down = md;
  md_up.volatility = md.volatility + bump;
  md_down.volatility = std::max(0.0, md.volatility - bump);

  double price_up = price(md_up);
  double price_down = price(md_down);

  double result = (price_up - price_down) / (2.0 * bump);

  if (std::isnan(result) || std::isinf(result)) {
    throw std::runtime_error("Invalid vega calculated");
  }

  return result;
}

double AmericanOption::theta(const MarketData &md) const {
  validateMarketData(md);

  const double bump = 1.0 / 365.0;

  if (time_to_expiry_years_ < bump) {
    return 0.0;
  }

  double current_price = price(md);

  AmericanOption temp_option = *this;
  temp_option.time_to_expiry_years_ =
      std::max(0.0, time_to_expiry_years_ - bump);
  double future_price = temp_option.price(md);

  double result = (future_price - current_price) / bump;

  if (std::isnan(result) || std::isinf(result)) {
    throw std::runtime_error("Invalid theta calculated");
  }

  return result;
}

std::string AmericanOption::getAssetId() const { return underlying_asset_id_; }