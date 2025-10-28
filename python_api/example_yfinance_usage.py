"""
Example Usage: YFinance Market Data Integration
Demonstrates how to use the new market data fetching capabilities
"""

from urllib import response
import requests
import json
from typing import List, Dict

# API Configuration
API_BASE_URL = "https://quant-enthusiasts-risk-engine.onrender.com"


def example_1_basic_fetch():
    """Example 1: Fetch market data for a single ticker"""
    print("=" * 70)
    print("Example 1: Basic Market Data Fetch")
    print("=" * 70)
    
    # Fetch data for AAPL
    response = requests.post(
        f"{API_BASE_URL}/update_market_data",
        json={"tickers": ["AAPL"]},
        headers={"Content-Type": "application/json"}
    )
    
    if response.status_code == 200:
        data = response.json()
        print(f"\n✓ {data['message']}")
    
    print()


def main():
    """Run all examples"""
    print("\n")
    print("=" * 70)
    print("YFINANCE MARKET DATA INTEGRATION - USAGE EXAMPLES")
    print("=" * 70)
    print("\nMake sure the Flask API is running on {API_BASE_URL}\n")
    
    try:
        # Check API health
        health_response = requests.get(f"{API_BASE_URL}/health", timeout=2)
        if health_response.status_code != 200:
            print("✗ Error: API is not responding correctly")
            return
        
        print("✓ API is healthy and ready\n")
        
        # Run examples
        examples = [
            example_1_basic_fetch,
            example_2_bulk_fetch,
            example_3_use_cached_data,
            example_4_force_refresh,
            example_5_view_cache,
            example_6_handle_errors,
            example_7_portfolio_workflow,
        ]
        
        for i, example in enumerate(examples, 1):
            input(f"Press Enter to run Example {i}...")
            try:
                example()
            except requests.exceptions.RequestException as e:
                print(f"\n✗ Network error: {e}\n")
            except Exception as e:
                print(f"\n✗ Unexpected error: {e}\n")
        
        print("=" * 70)
        print("All examples completed!")
        print("=" * 70)
        print()
        
    except requests.exceptions.ConnectionError:
        print("✗ Error: Cannot connect to API at", API_BASE_URL)
        print("Please make sure the Flask server is running:")
        print("  cd python_api && python app.py")
    except KeyboardInterrupt:
        print("\n\nExamples interrupted by user")


if __name__ == "__main__":
    main()

    data = response.json()
    if response.status_code == 200:
        print("\n✓ Successfully fetched AAPL data:")
        print(json.dumps(data["updated"]["AAPL"], indent=2))
    else:
        print(f"\n✗ Error: {data}")
    
    print()


def example_2_bulk_fetch():
    """Example 2: Fetch multiple tickers at once"""
    print("=" * 70)
    print("Example 2: Bulk Ticker Fetch")
    print("=" * 70)
    
    tickers = ["AAPL", "GOOGL", "MSFT", "AMZN", "TSLA"]
    
    response = requests.post(
        f"{API_BASE_URL}/update_market_data",
        json={"tickers": tickers},
        headers={"Content-Type": "application/json"}
    )
    
    if response.status_code in [200, 207]:
        data = response.json()
        print(f"\n✓ Successfully fetched {data['summary']['successful']}/{data['summary']['total_requested']} tickers")
        
        for ticker, market_data in data["updated"].items():
            print(f"\n{ticker}:")
            print(f"  Spot Price: ${market_data['spot']:.2f}")
            print(f"  Volatility: {market_data['vol']*100:.2f}%")
            print(f"  Rate: {market_data['rate']*100:.2f}%")
        
        if data["failed"]:
            print(f"\n✗ Failed to fetch {len(data['failed'])} tickers:")
            for failure in data["failed"]:
                print(f"  - {failure['ticker']}: {failure['error']}")
    
    print()


def example_3_use_cached_data():
    """Example 3: Use cached data in risk calculation"""
    print("=" * 70)
    print("Example 3: Use Cached Data in Risk Calculation")
    print("=" * 70)
    
    # First, update market data
    print("\nStep 1: Updating market data...")
    update_response = requests.post(
        f"{API_BASE_URL}/update_market_data",
        json={"tickers": ["AAPL"]},
        headers={"Content-Type": "application/json"}
    )
    
    if update_response.status_code != 200:
        print("✗ Failed to update market data")
        return
    
    print("✓ Market data updated")
    
    # Now calculate risk WITHOUT providing market data
    # The engine will use cached data automatically
    print("\nStep 2: Calculating risk using cached data...")
    
    portfolio = {
        "portfolio": [
            {
                "type": "call",
                "strike": 180,
                "expiry": 1.0,
                "asset_id": "AAPL",
                "quantity": 100,
                "style": "european"
            }
        ],
        "market_data": {}  # Empty! Will use cache
    }
    
    risk_response = requests.post(
        f"{API_BASE_URL}/calculate_risk",
        json=portfolio,
        headers={"Content-Type": "application/json"}
    )
    
    if risk_response.status_code == 200:
        data = risk_response.json()
        print("\n✓ Risk calculated using cached market data:")
        print(f"  Total PV: ${data['total_pv']:.2f}")
        print(f"  Delta: {data['total_delta']:.4f}")
        print(f"  VaR (95%): ${data['value_at_risk_95']:.2f}")
    else:
        print(f"\n✗ Error: {risk_response.json()}")
    
    print()


def example_4_force_refresh():
    """Example 4: Force refresh to get latest data"""
    print("=" * 70)
    print("Example 4: Force Refresh Market Data")
    print("=" * 70)
    
    # Fetch with force_refresh=True to bypass cache
    response = requests.post(
        f"{API_BASE_URL}/update_market_data",
        json={
            "tickers": ["AAPL"],
            "force_refresh": True
        },
        headers={"Content-Type": "application/json"}
    )
    
    if response.status_code == 200:
        data = response.json()
        print("\n✓ Fresh data fetched (cache bypassed):")
        print(json.dumps(data["updated"]["AAPL"], indent=2))
    
    print()


def example_5_view_cache():
    """Example 5: View all cached market data"""
    print("=" * 70)
    print("Example 5: View Cached Market Data")
    print("=" * 70)
    
    # Get all cached data
    response = requests.get(f"{API_BASE_URL}/get_cached_market_data")
    
    if response.status_code == 200:
        data = response.json()
        
        if not data:
            print("\n✗ No data in cache")
        else:
            print(f"\n✓ Found {len(data)} assets in cache:")
            for ticker, market_data in data.items():
                print(f"\n{ticker}:")
                print(f"  Spot: ${market_data['spot']:.2f}")
                print(f"  Vol: {market_data['vol']*100:.2f}%")
                print(f"  Last Updated: {market_data['last_updated']}")
    
    print()


def example_6_handle_errors():
    """Example 6: Error handling with invalid tickers"""
    print("=" * 70)
    print("Example 6: Error Handling")
    print("=" * 70)
    
    # Try to fetch invalid ticker
    response = requests.post(
        f"{API_BASE_URL}/update_market_data",
        json={"tickers": ["AAPL", "INVALID_XYZ", "GOOGL"]},
        headers={"Content-Type": "application/json"}
    )
    
    if response.status_code == 207:  # Partial success
        data = response.json()
        print(f"\n⚠ Partial success:")
        print(f"  Successful: {data['summary']['successful']}")
        print(f"  Failed: {data['summary']['failed']}")
        
        print("\n✓ Successfully fetched:")
        for ticker in data["updated"].keys():
            print(f"  - {ticker}")
        
        print("\n✗ Failed to fetch:")
        for failure in data["failed"]:
            print(f"  - {failure['ticker']}: {failure['error']}")
    
    print()


def example_7_portfolio_workflow():
    """Example 7: Complete workflow - fetch data and calculate risk"""
    print("=" * 70)
    print("Example 7: Complete Portfolio Workflow")
    print("=" * 70)
    
    # Define portfolio
    tickers = ["AAPL", "GOOGL", "MSFT"]
    
    # Step 1: Fetch market data
    print("\nStep 1: Fetching market data...")
    update_response = requests.post(
        f"{API_BASE_URL}/update_market_data",
        json={"tickers": tickers},
        headers={"Content-Type": "application/json"}
    )
    
    if update_response.status_code not in [200, 207]:
        print("✗ Failed to fetch market data")
        return
    
    print("✓ Market data fetched")
    
    # Step 2: Build portfolio
    print("\nStep 2: Building portfolio...")
    portfolio = {
        "portfolio": [
            {
                "type": "call",
                "strike": 180,
                "expiry": 1.0,
                "asset_id": "AAPL",
                "quantity": 100,
                "style": "european"
            },
            {
                "type": "put",
                "strike": 140,
                "expiry": 0.5,
                "asset_id": "GOOGL",
                "quantity": -50,
                "style": "european"
            },
            {
                "type": "call",
                "strike": 380,
                "expiry": 0.75,
                "asset_id": "MSFT",
                "quantity": 75,
                "style": "american"
            }
        ],
        "market_data": {},  # Will use cached data
        "var_parameters": {
            "simulations": 100000,
            "confidence": 0.95,
            "time_horizon": 1.0
        }
    }
    
    # Step 3: Calculate risk
    print("\nStep 3: Calculating portfolio risk...")
    risk_response = requests.post(
        f"{API_BASE_URL}/calculate_risk",
        json=portfolio,
        headers={"Content-Type": "application/json"}
    )
    
    if risk_response.status_code == 200:
        data = risk_response.json()
        print("\n✓ Portfolio Risk Metrics:")
        print(f"  Total PV: ${data['total_pv']:.2f}")
        print(f"  Delta: {data['total_delta']:.4f}")
        print(f"  Gamma: {data['total_gamma']:.4f}")
        print(f"  Vega: {data['total_vega']:.4f}")
        print(f"  Theta: {data['total_theta']:.4f}")
        print(f"  VaR (95%, 1-day): ${data['value_at_risk_95']:.2f}")
        print(f"  Portfolio Size: {data['portfolio_size']} instruments")
    else:
        print(f"\n✗ Error: {risk_response.json()}")
    
    print()

