#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <pybind11/stl_bind.h>
#include "Instrument.h"
#include "Portfolio.h"
#include "RiskEngine.h"
#include "MarketData.h"
#include <memory>

namespace py = pybind11;

PYBIND11_MODULE(quant_risk_engine, m)
{
    m.doc() = "Python bindings for the Quant Enthusiasts Risk Engine";

    py::enum_<OptionType>(m, "OptionType")
        .value("Call", OptionType::Call)
        .value("Put", OptionType::Put)
        .export_values();

    py::enum_<PricingModel>(m, "PricingModel")
        .value("BlackScholes", PricingModel::BlackScholes)
        .value("Binomial", PricingModel::Binomial)
        .value("MertonJumpDiffusion", PricingModel::MertonJumpDiffusion)
        .export_values();

    py::class_<MarketData>(m, "MarketData")
        .def(py::init<>())
        .def(py::init<std::string, double, double, double>(),
             py::arg("asset_id"), py::arg("spot"), py::arg("rate"), py::arg("vol"))
        .def(py::init<std::string, double, double, double, double>(),
             py::arg("asset_id"), py::arg("spot"), py::arg("rate"), py::arg("vol"), py::arg("div"))
        .def_readwrite("asset_id", &MarketData::asset_id)
        .def_readwrite("spot_price", &MarketData::spot_price)
        .def_readwrite("risk_free_rate", &MarketData::risk_free_rate)
        .def_readwrite("volatility", &MarketData::volatility)
        .def_readwrite("dividend_yield", &MarketData::dividend_yield)
        .def("validate", &MarketData::validate)
        .def("is_valid", &MarketData::isValid);

    py::class_<MarketDataManager>(m, "MarketDataManager")
        .def(py::init<>())
        .def("add_market_data", &MarketDataManager::addMarketData,
             py::arg("asset_id"), py::arg("market_data"))
        .def("update_market_data", &MarketDataManager::updateMarketData,
             py::arg("asset_id"), py::arg("market_data"))
        .def("get_market_data", &MarketDataManager::getMarketData,
             py::arg("asset_id"))
        .def("has_market_data", &MarketDataManager::hasMarketData,
             py::arg("asset_id"))
        .def("remove_market_data", &MarketDataManager::removeMarketData,
             py::arg("asset_id"))
        .def("clear", &MarketDataManager::clear)
        .def("size", &MarketDataManager::size)
        .def("get_all_market_data", &MarketDataManager::getAllMarketData)
        .def("__len__", &MarketDataManager::size);

    py::class_<Instrument, std::shared_ptr<Instrument>>(m, "Instrument")
        .def("price", &Instrument::price)
        .def("delta", &Instrument::delta)
        .def("gamma", &Instrument::gamma)
        .def("vega", &Instrument::vega)
        .def("theta", &Instrument::theta)
        .def("get_asset_id", &Instrument::getAssetId)
        .def("get_instrument_type", &Instrument::getInstrumentType)
        .def("is_valid", &Instrument::isValid);

    py::class_<EuropeanOption, Instrument, std::shared_ptr<EuropeanOption>>(m, "EuropeanOption")
        .def(py::init<OptionType, double, double, std::string>(),
             py::arg("option_type"), py::arg("strike"), py::arg("expiry"), py::arg("asset_id"))
        .def(py::init<OptionType, double, double, std::string, PricingModel>(),
             py::arg("option_type"), py::arg("strike"), py::arg("expiry"),
             py::arg("asset_id"), py::arg("pricing_model"))
        .def("set_pricing_model", &EuropeanOption::setPricingModel)
        .def("get_pricing_model", &EuropeanOption::getPricingModel)
        .def("set_binomial_steps", &EuropeanOption::setBinomialSteps)
        .def("get_binomial_steps", &EuropeanOption::getBinomialSteps)
        .def("set_jump_parameters", &EuropeanOption::setJumpParameters,
             py::arg("lambda"), py::arg("jump_mean"), py::arg("jump_vol"))
        .def("get_jump_intensity", &EuropeanOption::getJumpIntensity)
        .def("get_option_type", &EuropeanOption::getOptionType)
        .def("get_strike", &EuropeanOption::getStrike)
        .def("get_time_to_expiry", &EuropeanOption::getTimeToExpiry);

    py::class_<AmericanOption, Instrument, std::shared_ptr<AmericanOption>>(m, "AmericanOption")
        .def(py::init<OptionType, double, double, std::string>(),
             py::arg("option_type"), py::arg("strike"), py::arg("expiry"), py::arg("asset_id"))
        .def(py::init<OptionType, double, double, std::string, int>(),
             py::arg("option_type"), py::arg("strike"), py::arg("expiry"),
             py::arg("asset_id"), py::arg("binomial_steps"))
        .def("set_binomial_steps", &AmericanOption::setBinomialSteps)
        .def("get_binomial_steps", &AmericanOption::getBinomialSteps);

    py::class_<Portfolio>(m, "Portfolio")
        .def(py::init<>())
        .def("add_instrument", [](Portfolio &p, EuropeanOption &instr, int quantity)
             {
            auto owned_instr = std::make_unique<EuropeanOption>(instr);
            p.addInstrument(std::move(owned_instr), quantity); }, py::arg("instrument"), py::arg("quantity"))
        .def("add_instrument", [](Portfolio &p, AmericanOption &instr, int quantity)
             {
            auto owned_instr = std::make_unique<AmericanOption>(instr);
            p.addInstrument(std::move(owned_instr), quantity); }, py::arg("instrument"), py::arg("quantity"))
        .def("size", &Portfolio::size)
        .def("empty", &Portfolio::empty)
        .def("clear", &Portfolio::clear)
        .def("reserve", &Portfolio::reserve)
        .def("get_total_quantity", &Portfolio::getTotalQuantityForAsset)
        .def("remove_instrument", &Portfolio::removeInstrument)
        .def("update_quantity", &Portfolio::updateQuantity)
        .def("__len__", &Portfolio::size)
        .def("__bool__", [](const Portfolio &p)
             { return !p.empty(); });

    py::class_<PortfolioRiskResult>(m, "PortfolioRiskResult")
        .def(py::init<>())
        .def_readwrite("total_pv", &PortfolioRiskResult::total_pv)
        .def_readwrite("total_delta", &PortfolioRiskResult::total_delta)
        .def_readwrite("total_gamma", &PortfolioRiskResult::total_gamma)
        .def_readwrite("total_vega", &PortfolioRiskResult::total_vega)
        .def_readwrite("total_theta", &PortfolioRiskResult::total_theta)
        .def_readwrite("value_at_risk_95", &PortfolioRiskResult::value_at_risk_95)
        .def("is_valid", &PortfolioRiskResult::isValid)
        .def("reset", &PortfolioRiskResult::reset);

    py::class_<RiskEngine>(m, "RiskEngine")
        .def(py::init<>())
        .def(py::init<int>())
        .def("calculate_portfolio_risk", &RiskEngine::calculatePortfolioRisk)
        .def("set_var_simulations", &RiskEngine::setVaRSimulations)
        .def("get_var_simulations", &RiskEngine::getVaRSimulations)
        .def("set_var_confidence_level", &RiskEngine::setVaRConfidenceLevel)
        .def("get_var_confidence_level", &RiskEngine::getVaRConfidenceLevel)
        .def("set_var_time_horizon_days", &RiskEngine::setVaRTimeHorizonDays)
        .def("get_var_time_horizon_days", &RiskEngine::getVaRTimeHorizonDays)
        .def("set_random_seed", &RiskEngine::setRandomSeed)
        .def("set_use_fixed_seed", &RiskEngine::setUseFixedSeed);
}