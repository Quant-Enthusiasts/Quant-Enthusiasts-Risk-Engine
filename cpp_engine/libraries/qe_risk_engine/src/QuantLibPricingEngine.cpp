#include "QuantLibPricingEngine.h"

#ifdef USE_QUANTLIB

#include <ql/quantlib.hpp>
#include <cmath>
#include <sstream>
#include <iomanip>

using namespace QuantLib;

namespace QuantLibPricer {

// Helper function to convert our OptionType to QuantLib Option::Type
Option::Type toQLOptionType(OptionType type) {
    return (type == OptionType::Call) ? Option::Call : Option::Put;
}

// Helper to create QuantLib Date from years to expiry
Date createExpiryDate(double T) {
    Date today = Date::todaysDate();
    int days = static_cast<int>(T * 365.0);
    return today + days;
}

double europeanOptionBlackScholes(
    double S, double K, double r, double T, double sigma,
    OptionType type
) {
    try {
        // Set up QuantLib environment
        Date today = Date::todaysDate();
        Settings::instance().evaluationDate() = today;
        
        // Set up market data
        Handle<Quote> underlying_quote(ext::make_shared<SimpleQuote>(S));
        Handle<Quote> volatility_quote(ext::make_shared<SimpleQuote>(sigma));
        
        Handle<YieldTermStructure> risk_free_curve(
            ext::make_shared<FlatForward>(today, r, Actual365Fixed())
        );
        
        Handle<BlackVolTermStructure> volatility_curve(
            ext::make_shared<BlackConstantVol>(today, NullCalendar(), 
                                               volatility_quote, Actual365Fixed())
        );
        
        // Create Black-Scholes process
        ext::shared_ptr<BlackScholesMertonProcess> bs_process =
            ext::make_shared<BlackScholesMertonProcess>(
                underlying_quote,
                Handle<YieldTermStructure>(ext::make_shared<FlatForward>(today, 0.0, Actual365Fixed())),
                risk_free_curve,
                volatility_curve
            );
        
        // Create European option
        Date expiry = createExpiryDate(T);
        ext::shared_ptr<Exercise> european_exercise = 
            ext::make_shared<EuropeanExercise>(expiry);
        ext::shared_ptr<StrikedTypePayoff> payoff =
            ext::make_shared<PlainVanillaPayoff>(toQLOptionType(type), K);
        
        VanillaOption option(payoff, european_exercise);
        
        // Use analytical Black-Scholes engine
        option.setPricingEngine(
            ext::make_shared<AnalyticEuropeanEngine>(bs_process)
        );
        
        return option.NPV();
        
    } catch (const std::exception& e) {
        throw std::runtime_error(std::string("QuantLib Black-Scholes pricing error: ") + e.what());
    }
}

double europeanOptionBinomial(
    double S, double K, double r, double T, double sigma,
    OptionType type, int steps
) {
    try {
        Date today = Date::todaysDate();
        Settings::instance().evaluationDate() = today;
        
        Handle<Quote> underlying_quote(ext::make_shared<SimpleQuote>(S));
        Handle<Quote> volatility_quote(ext::make_shared<SimpleQuote>(sigma));
        
        Handle<YieldTermStructure> risk_free_curve(
            ext::make_shared<FlatForward>(today, r, Actual365Fixed())
        );
        
        Handle<BlackVolTermStructure> volatility_curve(
            ext::make_shared<BlackConstantVol>(today, NullCalendar(), 
                                               volatility_quote, Actual365Fixed())
        );
        
        ext::shared_ptr<BlackScholesMertonProcess> bs_process =
            ext::make_shared<BlackScholesMertonProcess>(
                underlying_quote,
                Handle<YieldTermStructure>(ext::make_shared<FlatForward>(today, 0.0, Actual365Fixed())),
                risk_free_curve,
                volatility_curve
            );
        
        Date expiry = createExpiryDate(T);
        ext::shared_ptr<Exercise> european_exercise = 
            ext::make_shared<EuropeanExercise>(expiry);
        ext::shared_ptr<StrikedTypePayoff> payoff =
            ext::make_shared<PlainVanillaPayoff>(toQLOptionType(type), K);
        
        VanillaOption option(payoff, european_exercise);
        
        // Use Binomial (Cox-Ross-Rubinstein) engine
        option.setPricingEngine(
            ext::make_shared<BinomialVanillaEngine<CoxRossRubinstein>>(bs_process, steps)
        );
        
        return option.NPV();
        
    } catch (const std::exception& e) {
        throw std::runtime_error(std::string("QuantLib Binomial pricing error: ") + e.what());
    }
}

double americanOptionBinomial(
    double S, double K, double r, double T, double sigma,
    OptionType type, int steps
) {
    try {
        Date today = Date::todaysDate();
        Settings::instance().evaluationDate() = today;
        
        Handle<Quote> underlying_quote(ext::make_shared<SimpleQuote>(S));
        Handle<Quote> volatility_quote(ext::make_shared<SimpleQuote>(sigma));
        
        Handle<YieldTermStructure> risk_free_curve(
            ext::make_shared<FlatForward>(today, r, Actual365Fixed())
        );
        
        Handle<BlackVolTermStructure> volatility_curve(
            ext::make_shared<BlackConstantVol>(today, NullCalendar(), 
                                               volatility_quote, Actual365Fixed())
        );
        
        ext::shared_ptr<BlackScholesMertonProcess> bs_process =
            ext::make_shared<BlackScholesMertonProcess>(
                underlying_quote,
                Handle<YieldTermStructure>(ext::make_shared<FlatForward>(today, 0.0, Actual365Fixed())),
                risk_free_curve,
                volatility_curve
            );
        
        Date expiry = createExpiryDate(T);
        ext::shared_ptr<Exercise> american_exercise = 
            ext::make_shared<AmericanExercise>(today, expiry);
        ext::shared_ptr<StrikedTypePayoff> payoff =
            ext::make_shared<PlainVanillaPayoff>(toQLOptionType(type), K);
        
        VanillaOption option(payoff, american_exercise);
        
        // Use Binomial engine for American option
        option.setPricingEngine(
            ext::make_shared<BinomialVanillaEngine<CoxRossRubinstein>>(bs_process, steps)
        );
        
        return option.NPV();
        
    } catch (const std::exception& e) {
        throw std::runtime_error(std::string("QuantLib American Binomial pricing error: ") + e.what());
    }
}

ValidationResult validateBlackScholesPrice(
    double internal_price,
    double S, double K, double r, double T, double sigma,
    OptionType type,
    double tolerance
) {
    ValidationResult result;
    result.internal_price = internal_price;
    
    try {
        result.quantlib_price = europeanOptionBlackScholes(S, K, r, T, sigma, type);
        result.absolute_diff = std::abs(internal_price - result.quantlib_price);
        
        // Calculate relative difference
        if (result.quantlib_price != 0.0) {
            result.relative_diff = result.absolute_diff / std::abs(result.quantlib_price);
        } else {
            result.relative_diff = (result.absolute_diff == 0.0) ? 0.0 : 1.0;
        }
        
        result.within_tolerance = (result.relative_diff <= tolerance);
        
        std::ostringstream oss;
        oss << std::fixed << std::setprecision(8);
        oss << "Internal: " << internal_price 
            << ", QuantLib: " << result.quantlib_price
            << ", Relative Diff: " << (result.relative_diff * 100.0) << "%";
        
        if (result.within_tolerance) {
            oss << " [PASS]";
        } else {
            oss << " [FAIL - exceeds tolerance " << (tolerance * 100.0) << "%]";
        }
        
        result.message = oss.str();
        
    } catch (const std::exception& e) {
        result.within_tolerance = false;
        result.message = std::string("Validation error: ") + e.what();
    }
    
    return result;
}

ValidationResult validateBinomialPriceEuropean(
    double internal_price,
    double S, double K, double r, double T, double sigma,
    OptionType type, int steps,
    double tolerance
) {
    ValidationResult result;
    result.internal_price = internal_price;
    
    try {
        result.quantlib_price = europeanOptionBinomial(S, K, r, T, sigma, type, steps);
        result.absolute_diff = std::abs(internal_price - result.quantlib_price);
        
        if (result.quantlib_price != 0.0) {
            result.relative_diff = result.absolute_diff / std::abs(result.quantlib_price);
        } else {
            result.relative_diff = (result.absolute_diff == 0.0) ? 0.0 : 1.0;
        }
        
        result.within_tolerance = (result.relative_diff <= tolerance);
        
        std::ostringstream oss;
        oss << std::fixed << std::setprecision(8);
        oss << "Internal: " << internal_price 
            << ", QuantLib: " << result.quantlib_price
            << ", Relative Diff: " << (result.relative_diff * 100.0) << "%";
        
        if (result.within_tolerance) {
            oss << " [PASS]";
        } else {
            oss << " [FAIL - exceeds tolerance " << (tolerance * 100.0) << "%]";
        }
        
        result.message = oss.str();
        
    } catch (const std::exception& e) {
        result.within_tolerance = false;
        result.message = std::string("Validation error: ") + e.what();
    }
    
    return result;
}

ValidationResult validateBinomialPriceAmerican(
    double internal_price,
    double S, double K, double r, double T, double sigma,
    OptionType type, int steps,
    double tolerance
) {
    ValidationResult result;
    result.internal_price = internal_price;
    
    try {
        result.quantlib_price = americanOptionBinomial(S, K, r, T, sigma, type, steps);
        result.absolute_diff = std::abs(internal_price - result.quantlib_price);
        
        if (result.quantlib_price != 0.0) {
            result.relative_diff = result.absolute_diff / std::abs(result.quantlib_price);
        } else {
            result.relative_diff = (result.absolute_diff == 0.0) ? 0.0 : 1.0;
        }
        
        result.within_tolerance = (result.relative_diff <= tolerance);
        
        std::ostringstream oss;
        oss << std::fixed << std::setprecision(8);
        oss << "Internal: " << internal_price 
            << ", QuantLib: " << result.quantlib_price
            << ", Relative Diff: " << (result.relative_diff * 100.0) << "%";
        
        if (result.within_tolerance) {
            oss << " [PASS]";
        } else {
            oss << " [FAIL - exceeds tolerance " << (tolerance * 100.0) << "%]";
        }
        
        result.message = oss.str();
        
    } catch (const std::exception& e) {
        result.within_tolerance = false;
        result.message = std::string("Validation error: ") + e.what();
    }
    
    return result;
}

GreeksResult calculateGreeks(
    double S, double K, double r, double T, double sigma,
    OptionType type
) {
    GreeksResult result = {0.0, 0.0, 0.0, 0.0, 0.0};
    
    try {
        Date today = Date::todaysDate();
        Settings::instance().evaluationDate() = today;
        
        Handle<Quote> underlying_quote(ext::make_shared<SimpleQuote>(S));
        Handle<Quote> volatility_quote(ext::make_shared<SimpleQuote>(sigma));
        
        Handle<YieldTermStructure> risk_free_curve(
            ext::make_shared<FlatForward>(today, r, Actual365Fixed())
        );
        
        Handle<BlackVolTermStructure> volatility_curve(
            ext::make_shared<BlackConstantVol>(today, NullCalendar(), 
                                               volatility_quote, Actual365Fixed())
        );
        
        ext::shared_ptr<BlackScholesMertonProcess> bs_process =
            ext::make_shared<BlackScholesMertonProcess>(
                underlying_quote,
                Handle<YieldTermStructure>(ext::make_shared<FlatForward>(today, 0.0, Actual365Fixed())),
                risk_free_curve,
                volatility_curve
            );
        
        Date expiry = createExpiryDate(T);
        ext::shared_ptr<Exercise> european_exercise = 
            ext::make_shared<EuropeanExercise>(expiry);
        ext::shared_ptr<StrikedTypePayoff> payoff =
            ext::make_shared<PlainVanillaPayoff>(toQLOptionType(type), K);
        
        VanillaOption option(payoff, european_exercise);
        option.setPricingEngine(ext::make_shared<AnalyticEuropeanEngine>(bs_process));
        
        result.delta = option.delta();
        result.gamma = option.gamma();
        result.vega = option.vega();
        result.theta = option.theta();
        result.rho = option.rho();
        
    } catch (const std::exception& e) {
        throw std::runtime_error(std::string("QuantLib Greeks calculation error: ") + e.what());
    }
    
    return result;
}

double barrierOptionPrice(
    double S, double K, double barrier, double r, double T, double sigma,
    OptionType option_type, BarrierType barrier_type,
    double rebate
) {
    try {
        Date today = Date::todaysDate();
        Settings::instance().evaluationDate() = today;
        
        Handle<Quote> underlying_quote(ext::make_shared<SimpleQuote>(S));
        Handle<Quote> volatility_quote(ext::make_shared<SimpleQuote>(sigma));
        
        Handle<YieldTermStructure> risk_free_curve(
            ext::make_shared<FlatForward>(today, r, Actual365Fixed())
        );
        
        Handle<BlackVolTermStructure> volatility_curve(
            ext::make_shared<BlackConstantVol>(today, NullCalendar(), 
                                               volatility_quote, Actual365Fixed())
        );
        
        ext::shared_ptr<BlackScholesMertonProcess> bs_process =
            ext::make_shared<BlackScholesMertonProcess>(
                underlying_quote,
                Handle<YieldTermStructure>(ext::make_shared<FlatForward>(today, 0.0, Actual365Fixed())),
                risk_free_curve,
                volatility_curve
            );
        
        // Map our barrier type to QuantLib barrier type
        Barrier::Type ql_barrier_type;
        switch (barrier_type) {
            case BarrierType::DownIn:
                ql_barrier_type = Barrier::DownIn;
                break;
            case BarrierType::DownOut:
                ql_barrier_type = Barrier::DownOut;
                break;
            case BarrierType::UpIn:
                ql_barrier_type = Barrier::UpIn;
                break;
            case BarrierType::UpOut:
                ql_barrier_type = Barrier::UpOut;
                break;
            default:
                throw std::invalid_argument("Invalid barrier type");
        }
        
        Date expiry = createExpiryDate(T);
        ext::shared_ptr<Exercise> european_exercise = 
            ext::make_shared<EuropeanExercise>(expiry);
        ext::shared_ptr<StrikedTypePayoff> payoff =
            ext::make_shared<PlainVanillaPayoff>(toQLOptionType(option_type), K);
        
        QuantLib::BarrierOption barrier_option(ql_barrier_type, barrier, rebate, 
                                                payoff, european_exercise);
        
        // Use analytical barrier option engine
        barrier_option.setPricingEngine(
            ext::make_shared<AnalyticBarrierEngine>(bs_process)
        );
        
        return barrier_option.NPV();
        
    } catch (const std::exception& e) {
        throw std::runtime_error(std::string("QuantLib Barrier option pricing error: ") + e.what());
    }
}

double asianOptionPrice(
    double S, double K, double r, double T, double sigma,
    OptionType option_type, AverageType average_type,
    int num_fixings, double running_sum, int past_fixings
) {
    try {
        Date today = Date::todaysDate();
        Settings::instance().evaluationDate() = today;
        
        Handle<Quote> underlying_quote(ext::make_shared<SimpleQuote>(S));
        Handle<Quote> volatility_quote(ext::make_shared<SimpleQuote>(sigma));
        
        Handle<YieldTermStructure> risk_free_curve(
            ext::make_shared<FlatForward>(today, r, Actual365Fixed())
        );
        
        Handle<BlackVolTermStructure> volatility_curve(
            ext::make_shared<BlackConstantVol>(today, NullCalendar(), 
                                               volatility_quote, Actual365Fixed())
        );
        
        ext::shared_ptr<BlackScholesMertonProcess> bs_process =
            ext::make_shared<BlackScholesMertonProcess>(
                underlying_quote,
                Handle<YieldTermStructure>(ext::make_shared<FlatForward>(today, 0.0, Actual365Fixed())),
                risk_free_curve,
                volatility_curve
            );
        
        Date expiry = createExpiryDate(T);
        
        // Create fixing dates (evenly spaced)
        std::vector<Date> fixing_dates;
        int days_between = static_cast<int>((T * 365.0) / num_fixings);
        for (int i = 0; i < num_fixings; ++i) {
            fixing_dates.push_back(today + (i * days_between));
        }
        
        // Map average type
        Average::Type ql_average_type = (average_type == AverageType::Arithmetic) 
            ? Average::Arithmetic 
            : Average::Geometric;
        
        ext::shared_ptr<StrikedTypePayoff> payoff =
            ext::make_shared<PlainVanillaPayoff>(toQLOptionType(option_type), K);
        
        ext::shared_ptr<Exercise> european_exercise = 
            ext::make_shared<EuropeanExercise>(expiry);
        
        // Create discrete geometric Asian option
        DiscreteAveragingAsianOption asian_option(
            ql_average_type,
            running_sum,
            past_fixings,
            fixing_dates,
            payoff,
            european_exercise
        );
        
        // Use appropriate engine based on average type
        if (average_type == AverageType::Geometric) {
            asian_option.setPricingEngine(
                ext::make_shared<AnalyticDiscreteGeometricAveragePriceAsianEngine>(bs_process)
            );
        } else {
            // For arithmetic, use Monte Carlo engine
            asian_option.setPricingEngine(
                ext::make_shared<MCDiscreteArithmeticAPEngine<>>(
                    bs_process,
                    false, // brownian bridge
                    true,  // antithetic for variance reduction
                    true,  // control variate to improve convergence
                    5000,  // required samples
                    0.02,  // required tolerance (2%)
                    200000,// max samples safeguard
                    42     // seed
                )
            );
        }
        
        return asian_option.NPV();
        
    } catch (const std::exception& e) {
        throw std::runtime_error(std::string("QuantLib Asian option pricing error: ") + e.what());
    }
}

} // namespace QuantLibPricer

#endif // USE_QUANTLIB
