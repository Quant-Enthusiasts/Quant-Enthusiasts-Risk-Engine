#!/usr/bin/env python3
"""
Test script for the installed quant_risk_engine module.
This assumes the module has been installed to cpp_engine/install/lib/
"""

import sys
import os
from pathlib import Path

# Add the install directory to Python path
repo_root = Path(__file__).parent
install_lib_path = repo_root / "cpp_engine" / "install" / "lib"

if not install_lib_path.exists():
    print(f"ERROR: Installation directory not found: {install_lib_path}")
    print("Please build and install the project first:")
    print("  cd cpp_engine/build")
    print("  cmake --build .")
    print("  cmake --install .")
    sys.exit(1)

sys.path.insert(0, str(install_lib_path))

try:
    import quant_risk_engine
    print(f" Successfully imported quant_risk_engine from: {install_lib_path}")
except ImportError as e:
    print(f" Failed to import quant_risk_engine: {e}")
    sys.exit(1)

print("\n" + "="*60)
print("Testing Quant Risk Engine - Basic Functionality")
print("="*60 + "\n")

# Test 1: Create Market Data
print("Test 1: Creating Market Data")
try:
    market_data = quant_risk_engine.MarketData(
        "AAPL",     # asset_id
        150.0,      # spot price
        0.05,       # risk-free rate
        0.25,       # volatility
        0.01        # dividend yield
    )
    print(f" Created MarketData for AAPL")
    print(f"  Asset ID: {market_data.asset_id}")
    print(f"  Spot: ${market_data.spot_price:.2f}")
    print(f"  Rate: {market_data.risk_free_rate*100:.2f}%")
    print(f"  Vol: {market_data.volatility*100:.2f}%")
    print(f"  Dividend: {market_data.dividend_yield*100:.2f}%")
    print(f"  Is Valid: {market_data.is_valid()}")
except Exception as e:
    print(f" Failed to create MarketData: {e}")
    sys.exit(1)

# Test 2: Create European Call Option
print("\nTest 2: Creating European Call Option")
try:
    call_option = quant_risk_engine.EuropeanOption(
        quant_risk_engine.OptionType.Call,
        155.0,      # strike
        0.25,       # expiry (in years)
        "AAPL",     # asset_id
        quant_risk_engine.PricingModel.BlackScholes
    )
    print(f" Created European Call Option")
    print(f"  Type: {call_option.get_option_type()}")
    print(f"  Strike: ${call_option.get_strike():.2f}")
    print(f"  Expiry: {call_option.get_time_to_expiry():.2f} years")
    print(f"  Asset: {call_option.get_asset_id()}")
    print(f"  Pricing Model: {call_option.get_pricing_model()}")
except Exception as e:
    print(f" Failed to create European Call Option: {e}")
    sys.exit(1)

# Test 3: Price the Option
print("\nTest 3: Pricing the Call Option")
try:
    price = call_option.price(market_data)
    delta = call_option.delta(market_data)
    gamma = call_option.gamma(market_data)
    vega = call_option.vega(market_data)
    theta = call_option.theta(market_data)

    print(f" Option Pricing Results:")
    print(f"  Price: ${price:.4f}")
    print(f"  Delta: {delta:.4f}")
    print(f"  Gamma: {gamma:.4f}")
    print(f"  Vega: {vega:.4f}")
    print(f"  Theta: {theta:.4f}")
except Exception as e:
    print(f" Failed to price option: {e}")
    sys.exit(1)

# Test 4: Create European Put Option
print("\nTest 4: Creating European Put Option")
try:
    put_option = quant_risk_engine.EuropeanOption(
        quant_risk_engine.OptionType.Put,
        145.0,      # strike
        0.25,       # expiry (in years)
        "AAPL",     # asset_id
        quant_risk_engine.PricingModel.BlackScholes
    )
    put_price = put_option.price(market_data)
    put_delta = put_option.delta(market_data)

    print(f" Created and priced European Put Option")
    print(f"  Strike: ${put_option.get_strike()}")
    print(f"  Price: ${put_price:.4f}")
    print(f"  Delta: {put_delta:.4f}")
except Exception as e:
    print(f" Failed to create Put Option: {e}")
    sys.exit(1)

# Test 5: Create a Portfolio
print("\nTest 5: Creating Portfolio")
try:
    portfolio = quant_risk_engine.Portfolio()

    # Add the call option (long 10 contracts)
    portfolio.add_instrument(call_option, 10)

    # Add the put option (short 5 contracts)
    portfolio.add_instrument(put_option, -5)

    portfolio_size = len(portfolio)
    net_position = portfolio.get_total_quantity("AAPL")

    print(f" Created Portfolio")
    print(f"  Portfolio Size: {portfolio_size} instruments")
    print(f"  Net Position for AAPL: {net_position} contracts")
except Exception as e:
    print(f" Failed to create Portfolio: {e}")
    sys.exit(1)

# Test 6: Calculate Portfolio Risk
print("\nTest 6: Calculating Portfolio Risk")
try:
    # Create market data dictionary (as expected by calculate_portfolio_risk)
    market_data_dict = {"AAPL": market_data}

    # Create risk engine
    engine = quant_risk_engine.RiskEngine()
    engine.set_var_simulations(10000)
    engine.set_var_time_horizon_days(1.0)

    # Calculate risk - expects Portfolio and dict[str, MarketData]
    risk_result = engine.calculate_portfolio_risk(portfolio, market_data_dict)

    if risk_result.is_valid():
        print(f" Portfolio Risk Calculation Complete")
        print(f"  Total PV: ${risk_result.total_pv:.2f}")
        print(f"  Total Delta: {risk_result.total_delta:.4f}")
        print(f"  Total Gamma: {risk_result.total_gamma:.4f}")
        print(f"  Total Vega: {risk_result.total_vega:.4f}")
        print(f"  Total Theta: {risk_result.total_theta:.4f}")
        print(f"  VaR (95%): ${risk_result.value_at_risk_95:.2f}")
        print(f"  VaR (99%): ${risk_result.value_at_risk_99:.2f}")
        print(f"  Expected Shortfall (95%): ${risk_result.expected_shortfall_95:.2f}")
        print(f"  Expected Shortfall (99%): ${risk_result.expected_shortfall_99:.2f}")
    else:
        print(f" Risk calculation returned invalid results")
        sys.exit(1)
except Exception as e:
    print(f" Failed to calculate portfolio risk: {e}")
    import traceback
    traceback.print_exc()
    sys.exit(1)

# Test 7: Test American Option
print("\nTest 7: Creating American Option")
try:
    american_put = quant_risk_engine.AmericanOption(
        quant_risk_engine.OptionType.Put,
        145.0,      # strike
        0.5,        # expiry (in years)
        "AAPL",     # asset_id
        100         # binomial steps
    )
    american_price = american_put.price(market_data)
    american_delta = american_put.delta(market_data)
    american_gamma = american_put.gamma(market_data)
    american_vega = american_put.vega(market_data)
    american_theta = american_put.theta(market_data)

    print(f" Created and priced American Put Option")
    print(f"  Asset: {american_put.get_asset_id()}")
    print(f"  Type: {american_put.get_instrument_type()}")
    print(f"  Price: ${american_price:.4f}")
    print(f"  Delta: {american_delta:.4f}")
    print(f"  Gamma: {american_gamma:.4f}")
    print(f"  Vega: {american_vega:.4f}")
    print(f"  Theta: {american_theta:.4f}")
    print(f"  Binomial Steps: {american_put.get_binomial_steps()}")
except Exception as e:
    print(f" Failed to create American Option: {e}")
    import traceback
    traceback.print_exc()
    sys.exit(1)

# Test 8: Test Alternative Pricing Models
print("\nTest 8: Testing Alternative Pricing Models")
try:
    # Binomial model
    binomial_option = quant_risk_engine.EuropeanOption(
        quant_risk_engine.OptionType.Call,
        150.0,
        0.25,
        "AAPL",
        quant_risk_engine.PricingModel.Binomial
    )
    binomial_option.set_binomial_steps(100)
    binomial_price = binomial_option.price(market_data)

    # Jump Diffusion model
    jump_option = quant_risk_engine.EuropeanOption(
        quant_risk_engine.OptionType.Call,
        150.0,
        0.25,
        "AAPL",
        quant_risk_engine.PricingModel.MertonJumpDiffusion
    )
    jump_option.set_jump_parameters(2.0, -0.05, 0.15)  # lambda, mean, vol
    jump_price = jump_option.price(market_data)

    print(f" Alternative Pricing Models")
    print(f"  Binomial Model:")
    print(f"    Price: ${binomial_price:.4f}")
    print(f"    Steps: {binomial_option.get_binomial_steps()}")
    print(f"  Jump Diffusion Model:")
    print(f"    Price: ${jump_price:.4f}")
    print(f"    Jump Intensity: {jump_option.get_jump_intensity():.2f}")
    print(f"  Black-Scholes Model:")
    print(f"    Price: ${price:.4f}")
except Exception as e:
    print(f" Failed to test alternative pricing models: {e}")
    sys.exit(1)

# Test 9: Test MarketDataManager
print("\nTest 9: Testing MarketDataManager")
try:
    md_manager = quant_risk_engine.MarketDataManager()

    # Add market data for multiple assets
    aapl_md = quant_risk_engine.MarketData("AAPL", 150.0, 0.05, 0.25, 0.01)
    msft_md = quant_risk_engine.MarketData("MSFT", 350.0, 0.05, 0.28, 0.015)

    md_manager.add_market_data("AAPL", aapl_md)
    md_manager.add_market_data("MSFT", msft_md)

    print(f" MarketDataManager functionality")
    print(f"  Size: {len(md_manager)}")
    print(f"  Has AAPL: {md_manager.has_market_data('AAPL')}")
    print(f"  Has MSFT: {md_manager.has_market_data('MSFT')}")
    print(f"  Has GOOGL: {md_manager.has_market_data('GOOGL')}")

    # Retrieve data
    retrieved_aapl = md_manager.get_market_data("AAPL")
    print(f"  Retrieved AAPL spot: ${retrieved_aapl.spot_price:.2f}")

    # Update data
    updated_aapl = quant_risk_engine.MarketData("AAPL", 155.0, 0.05, 0.26, 0.01)
    md_manager.update_market_data("AAPL", updated_aapl)
    print(f"  Updated AAPL spot: ${md_manager.get_market_data('AAPL').spot_price:.2f}")

    # Get all data
    all_data = md_manager.get_all_market_data()
    print(f"  All assets in manager: {list(all_data.keys())}")

except Exception as e:
    print(f" Failed to test MarketDataManager: {e}")
    import traceback
    traceback.print_exc()
    sys.exit(1)

print("\n" + "="*60)
print("All Tests Passed Successfully! ")
print("="*60)
print(f"\nModule Location: {install_lib_path}")
print("The quant_risk_engine module is working correctly.")
