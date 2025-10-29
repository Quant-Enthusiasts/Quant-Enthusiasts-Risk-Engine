import React from "react";

export const APISection: React.FC = () => (
  <div className="space-y-6">
    <h2 className="text-3xl font-bold text-cyan-400 mb-6">API Reference</h2>

    <div className="mb-8">
      <p className="text-gray-300 mb-4">
        Complete REST API documentation for the Risk Engine.
      </p>
      <div className="bg-blue-900/20 border border-blue-500/50 rounded-lg p-4">
        <p className="text-blue-300 font-semibold mb-2">Base URL</p>
        <code className="text-sm text-blue-200">http://127.0.0.1:5000</code>
      </div>
    </div>

    <div className="mb-8">
      <h3 className="text-xl font-bold text-gray-100 mb-4">Health Check</h3>
      <span className="inline-block px-3 py-1 rounded-full text-xs font-semibold uppercase tracking-wide bg-green-500/20 text-green-400 mb-4">
        GET
      </span>{" "}
      <code className="text-cyan-400">/health</code>
      <p className="text-sm text-gray-400 mt-4 mb-3">
        Check API status and cache information
      </p>
      <h4 className="text-sm font-semibold text-gray-300 mb-2">
        Response (200)
      </h4>
      <div className="code-block" style={{ whiteSpace: "pre" }}>
        {`{
  "status": "healthy",
  "service": "quant-risk-engine",
  "version": "3.1",
  "features": [
    "european_options",
    "american_options",
    "multiple_pricing_models",
    "portfolio_analytics",
    "var_calculation",
    "live_market_data"
  ],
  "cache_info": {
    "cached_assets": 5,
    "cache_location": "market_data_cache.db"
  }
}`}
      </div>
    </div>

    <div className="mb-8">
      <h3 className="text-xl font-bold text-gray-100 mb-4">Price Option</h3>
      <span className="inline-block px-3 py-1 rounded-full text-xs font-semibold uppercase tracking-wide bg-blue-500/20 text-blue-400 mb-4">
        POST
      </span>{" "}
      <code className="text-cyan-400">/price_option</code>
      <p className="text-sm text-gray-400 mt-4 mb-3">
        Calculate option price and Greeks for a single instrument
      </p>
      <h4 className="text-sm font-semibold text-gray-300 mb-2">Request Body</h4>
      <div className="code-block mb-4" style={{ whiteSpace: "pre" }}>
        {`{
  "type": "call",
  "strike": 100.0,
  "expiry": 1.0,
  "asset_id": "AAPL",
  "style": "european",
  "pricing_model": "blackscholes",
  "market_data": {
    "spot": 105.0,
    "rate": 0.05,
    "vol": 0.25,
    "dividend": 0.01
  }
}`}
      </div>
      <h4 className="text-sm font-semibold text-gray-300 mb-2">
        Response (200)
      </h4>
      <div className="code-block" style={{ whiteSpace: "pre" }}>
        {`{
  "price": 12.34,
  "delta": 0.6368,
  "gamma": 0.0178,
  "vega": 37.45,
  "theta": -6.42,
  "rho": 48.23,
  "instrument_type": "EuropeanOption",
  "market_data_auto_fetched": false
}`}
      </div>
    </div>

    <div className="mb-8">
      <h3 className="text-xl font-bold text-gray-100 mb-4">
        Calculate Portfolio Risk
      </h3>
      <span className="inline-block px-3 py-1 rounded-full text-xs font-semibold uppercase tracking-wide bg-blue-500/20 text-blue-400 mb-4">
        POST
      </span>{" "}
      <code className="text-cyan-400">/calculate_risk</code>
      <p className="text-sm text-gray-400 mt-4 mb-3">
        Calculate comprehensive risk metrics for a portfolio
      </p>
      <h4 className="text-sm font-semibold text-gray-300 mb-2">Request Body</h4>
      <div className="code-block mb-4" style={{ whiteSpace: "pre" }}>
        {`{
  "portfolio": [
    {
      "type": "call",
      "strike": 100.0,
      "expiry": 1.0,
      "asset_id": "AAPL",
      "quantity": 100,
      "style": "european"
    }
  ],
  "market_data": {},
  "var_parameters": {
    "simulations": 100000,
    "confidence": 0.95,
    "time_horizon": 1.0
  }
}`}
      </div>
      <h4 className="text-sm font-semibold text-gray-300 mb-2">
        Response (200)
      </h4>
      <div className="code-block" style={{ whiteSpace: "pre" }}>
        {`{
  "total_pv": 123456.78,
  "total_delta": 0.6543,
  "total_gamma": 0.0234,
  "total_vega": 456.78,
  "total_theta": -12.34,
  "value_at_risk_95": -5678.90,
  "value_at_risk_99": -7890.12,
  "expected_shortfall_95": -6543.21,
  "portfolio_size": 1
}`}
      </div>
    </div>

    <div className="mb-8">
      <h3 className="text-xl font-bold text-gray-100 mb-4">
        Update Market Data
      </h3>
      <span className="inline-block px-3 py-1 rounded-full text-xs font-semibold uppercase tracking-wide bg-blue-500/20 text-blue-400 mb-4">
        POST
      </span>{" "}
      <code className="text-cyan-400">/update_market_data</code>
      <p className="text-sm text-gray-400 mt-4 mb-3">
        Fetch live market data from Yahoo Finance
      </p>
      <h4 className="text-sm font-semibold text-gray-300 mb-2">Request Body</h4>
      <div className="code-block mb-4" style={{ whiteSpace: "pre" }}>
        {`{
  "tickers": ["AAPL", "GOOGL", "MSFT"],
  "force_refresh": false
}`}
      </div>
      <h4 className="text-sm font-semibold text-gray-300 mb-2">
        Response (200)
      </h4>
      <div className="code-block" style={{ whiteSpace: "pre" }}>
        {`{
  "success": true,
  "updated": {
    "AAPL": {
      "asset_id": "AAPL",
      "spot": 175.43,
      "vol": 0.2847,
      "rate": 0.0445,
      "dividend": 0.0052,
      "last_updated": "2025-10-25T14:30:00.123456",
      "source": "yfinance"
    }
  },
  "failed": [],
  "summary": {
    "total_requested": 3,
    "successful": 3,
    "failed": 0
  }
}`}
      </div>
    </div>

    <div>
      <h3 className="text-xl font-bold text-gray-100 mb-4">Error Responses</h3>
      <p className="text-sm text-gray-400 mb-3">
        All endpoints follow consistent error format
      </p>

      <h4 className="text-sm font-semibold text-gray-300 mb-2">
        400 Bad Request
      </h4>
      <div className="code-block mb-4" style={{ whiteSpace: "pre" }}>
        {`{
  "error": "Validation error: Portfolio item 0: missing required field 'strike'"
}`}
      </div>

      <h4 className="text-sm font-semibold text-gray-300 mb-2">
        500 Internal Server Error
      </h4>
      <div className="code-block" style={{ whiteSpace: "pre" }}>
        {`{
  "error": "Runtime error: Risk calculation produced invalid results"
}`}
      </div>
    </div>
  </div>
);


