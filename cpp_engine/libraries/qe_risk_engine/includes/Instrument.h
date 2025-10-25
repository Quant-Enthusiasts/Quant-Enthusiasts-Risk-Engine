#ifndef INSTRUMENT_H
#define INSTRUMENT_H

#include "MarketData.h"
#include <string>
#include <stdexcept>
#include <memory>

enum class OptionType { Call, Put };

enum class PricingModel { 
    BlackScholes, 
    Binomial, 
    MertonJumpDiffusion 
};

class Instrument {
public:
    virtual ~Instrument() = default;
    
    virtual double price(const MarketData& md) const = 0;
    virtual double delta(const MarketData& md) const = 0;
    virtual double gamma(const MarketData& md) const = 0;
    virtual double vega(const MarketData& md) const = 0;
    virtual double theta(const MarketData& md) const = 0;
    virtual std::string getAssetId() const = 0;
    
    virtual std::string getInstrumentType() const = 0;
    virtual bool isValid() const = 0;
};

class EuropeanOption : public Instrument {
public:
    EuropeanOption(
        OptionType type, 
        double strike, 
        double time_to_expiry, 
        std::string asset_id
    );
    
    EuropeanOption(
        OptionType type, 
        double strike, 
        double time_to_expiry, 
        std::string asset_id,
        PricingModel model
    );
    
    double price(const MarketData& md) const override;
    double delta(const MarketData& md) const override;
    double gamma(const MarketData& md) const override;
    double vega(const MarketData& md) const override;
    double theta(const MarketData& md) const override;
    std::string getAssetId() const override;
    std::string getInstrumentType() const override;
    bool isValid() const override;
    
    void setPricingModel(PricingModel model);
    PricingModel getPricingModel() const;
    
    void setBinomialSteps(int steps);
    int getBinomialSteps() const;
    
    void setJumpParameters(double lambda, double jump_mean, double jump_vol);
    double getJumpIntensity() const;
    
    OptionType getOptionType() const;
    double getStrike() const;
    double getTimeToExpiry() const;

private:
    OptionType option_type_;
    double strike_price_;
    double time_to_expiry_years_;
    std::string underlying_asset_id_;
    PricingModel pricing_model_;
    
    int binomial_steps_;
    double jump_intensity_;
    double jump_mean_;
    double jump_volatility_;
    
    void validateParameters() const;
    void validateMarketData(const MarketData& md) const;
    
    double priceBlackScholes(const MarketData& md) const;
    double priceBinomial(const MarketData& md) const;
    double priceJumpDiffusion(const MarketData& md) const;
    
    double deltaBlackScholes(const MarketData& md) const;
    double deltaNumerical(const MarketData& md) const;
};

class AmericanOption : public Instrument {
public:
    AmericanOption(
        OptionType type,
        double strike,
        double time_to_expiry,
        std::string asset_id,
        int binomial_steps = 100
    );
    
    double price(const MarketData& md) const override;
    double delta(const MarketData& md) const override;
    double gamma(const MarketData& md) const override;
    double vega(const MarketData& md) const override;
    double theta(const MarketData& md) const override;
    std::string getAssetId() const override;
    std::string getInstrumentType() const override;
    bool isValid() const override;
    
    void setBinomialSteps(int steps);
    int getBinomialSteps() const;

private:
    OptionType option_type_;
    double strike_price_;
    double time_to_expiry_years_;
    std::string underlying_asset_id_;
    int binomial_steps_;
    
    void validateParameters() const;
    void validateMarketData(const MarketData& md) const;
    double calculateIntrinsicValue(double spot_price) const;
};

#endif