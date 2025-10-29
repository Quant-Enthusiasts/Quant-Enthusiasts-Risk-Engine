import React from "react";

export const MarketDataSection: React.FC = () => (
  <div className="space-y-6">
    <h2 className="text-3xl font-bold text-cyan-400 mb-6">
      Market Data Integration
    </h2>

    <div className="mb-8">
      <h3 className="text-xl font-bold text-gray-100 mb-4">Data Sources</h3>
      <p className="text-gray-300 mb-4">
        Live market data automatically fetched from Yahoo Finance with
        intelligent caching.
      </p>

      <div className="space-y-4">
        <div className="bg-gray-800/40 p-5 rounded-lg border border-gray-700/60">
          <h4 className="text-cyan-400 font-bold mb-2">Spot Price</h4>
          <p className="text-sm text-gray-400">
            Current market price or last closing price if market closed
          </p>
        </div>

        <div className="bg-gray-800/40 p-5 rounded-lg border border-gray-700/60">
          <h4 className="text-cyan-400 font-bold mb-2">Volatility</h4>
          <p className="text-sm text-gray-400">
            Calculated from 252 days historical data using log returns
          </p>
          <p className="text-xs text-gray-500 mt-1">
            Formula: σ = std(log(P_t / P_{"{t-1}"})) × sqrt(252)
          </p>
        </div>

        <div className="bg-gray-800/40 p-5 rounded-lg border border-gray-700/60">
          <h4 className="text-cyan-400 font-bold mb-2">Risk-Free Rate</h4>
          <p className="text-sm text-gray-400">
            10-year US Treasury rate (^TNX), default 4.5% if unavailable
          </p>
        </div>

        <div className="bg-gray-800/40 p-5 rounded-lg border border-gray-700/60">
          <h4 className="text-cyan-400 font-bold mb-2">Dividend Yield</h4>
          <p className="text-sm text-gray-400">
            Annual dividend yield from Yahoo Finance, default 0.0
          </p>
        </div>
      </div>
    </div>

    <div className="mb-8">
      <h3 className="text-xl font-bold text-gray-100 mb-4">Caching System</h3>
      <p className="text-gray-300 mb-4">
        SQLite-based cache reduces API calls and improves response times.
      </p>

      <div>
        <h4 className="text-sm font-semibold text-gray-300 mb-2">
          Cache Behavior
        </h4>
        <table
          className="mb-4 w-full border-separate"
          style={{ borderSpacing: 0, margin: "1.5rem 0" }}
        >
          <thead>
            <tr>
              <th className="bg-gray-900/90 p-3 text-left font-semibold text-cyan-400 border-b-2 border-cyan-400/30">
                Scenario
              </th>
              <th className="bg-gray-900/90 p-3 text-left font-semibold text-cyan-400 border-b-2 border-cyan-400/30">
                Action
              </th>
            </tr>
          </thead>
          <tbody>
            <tr className="hover:bg-cyan-500/5 transition-colors">
              <td className="p-3 border-b border-white/5 text-gray-300">
                Data age &lt; 24 hours
              </td>
              <td className="p-3 border-b border-white/5 text-gray-300">
                Return cached data (no network request)
              </td>
            </tr>
            <tr className="hover:bg-cyan-500/5 transition-colors">
              <td className="p-3 border-b border-white/5 text-gray-300">
                Data age &gt; 24 hours
              </td>
              <td className="p-3 border-b border-white/5 text-gray-300">
                Fetch fresh data, update cache
              </td>
            </tr>
            <tr className="hover:bg-cyan-500/5 transition-colors">
              <td className="p-3 border-b border-white/5 text-gray-300">
                Asset not in cache
              </td>
              <td className="p-3 border-b border-white/5 text-gray-300">
                Fetch from YFinance, store in cache
              </td>
            </tr>
            <tr className="hover:bg-cyan-500/5 transition-colors">
              <td className="p-3 border-b border-white/5 text-gray-300">
                force_refresh = true
              </td>
              <td className="p-3 border-b border-white/5 text-gray-300">
                Always fetch fresh data, bypass cache
              </td>
            </tr>
          </tbody>
        </table>
      </div>
      <h4 className="text-sm font-semibold text-gray-300 mb-2">
        Configuration
      </h4>
      <div className="code-block" style={{ whiteSpace: "pre" }}>
        {`# Environment Variables
MARKET_DATA_CACHE_HOURS=24
DEFAULT_RISK_FREE_RATE=0.045
MARKET_DATA_CACHE_PATH=./market_data_cache.db`}
      </div>
    </div>

    <div className="mb-8">
      <h3 className="text-xl font-bold text-gray-100 mb-4">Usage Patterns</h3>

      <h4 className="text-sm font-semibold text-gray-300 mb-2">
        Automatic Fetching
      </h4>
      <p className="text-sm text-gray-400 mb-3">
        Provide empty market_data object to trigger auto-fetch
      </p>
      <div className="code-block mb-4" style={{ whiteSpace: "pre" }}>
        {`{
  "portfolio": [...],
  "market_data": {} // Auto-fetches missing data
}`}
      </div>

      <h4 className="text-sm font-semibold text-gray-300 mb-2">Manual Data</h4>
      <p className="text-sm text-gray-400 mb-3">
        Override with your own market data
      </p>
      <div className="code-block mb-4" style={{ whiteSpace: "pre" }}>
        {`{
  "market_data": {
    "AAPL": {
      "spot": 175.0,
      "vol": 0.28,
      "rate": 0.045,
      "dividend": 0.005
    }
  }
}`}
      </div>

      <h4 className="text-sm font-semibold text-gray-300 mb-2">Bulk Update</h4>
      <p className="text-sm text-gray-400 mb-3">
        Pre-populate cache for multiple assets
      </p>
      <div className="code-block" style={{ whiteSpace: "pre" }}>
        {`curl -X POST /update_market_data \\
  -d '{"tickers": ["AAPL", "GOOGL", "MSFT"], "force_refresh": true}'`}
      </div>
    </div>

    <div>
      <h3 className="text-xl font-bold text-gray-100 mb-4">Error Handling</h3>
      <p className="text-gray-300 mb-4">
        Graceful degradation when market data unavailable.
      </p>

      <div>
        <table
          className="w-full border-separate"
          style={{ borderSpacing: 0, margin: "1.5rem 0" }}
        >
          <thead>
            <tr>
              <th className="bg-gray-900/90 p-3 text-left font-semibold text-cyan-400 border-b-2 border-cyan-400/30">
                Error
              </th>
              <th className="bg-gray-900/90 p-3 text-left font-semibold text-cyan-400 border-b-2 border-cyan-400/30">
                Cause
              </th>
              <th className="bg-gray-900/90 p-3 text-left font-semibold text-cyan-400 border-b-2 border-cyan-400/30">
                Solution
              </th>
            </tr>
          </thead>
          <tbody>
            <tr className="hover:bg-cyan-500/5 transition-colors">
              <td className="p-3 border-b border-white/5 text-gray-300">
                No price data
              </td>
              <td className="p-3 border-b border-white/5 text-gray-300">
                Invalid/delisted ticker
              </td>
              <td className="p-3 border-b border-white/5 text-gray-300">
                Verify on finance.yahoo.com
              </td>
            </tr>
            <tr className="hover:bg-cyan-500/5 transition-colors">
              <td className="p-3 border-b border-white/5 text-gray-300">
                Connection timeout
              </td>
              <td className="p-3 border-b border-white/5 text-gray-300">
                Network issues
              </td>
              <td className="p-3 border-b border-white/5 text-gray-300">
                Check connectivity, retry
              </td>
            </tr>
            <tr className="hover:bg-cyan-500/5 transition-colors">
              <td className="p-3 border-b border-white/5 text-gray-300">
                Insufficient history
              </td>
              <td className="p-3 border-b border-white/5 text-gray-300">
                Recent IPO
              </td>
              <td className="p-3 border-b border-white/5 text-gray-300">
                Manually specify volatility
              </td>
            </tr>
            <tr className="hover:bg-cyan-500/5 transition-colors">
              <td className="p-3 border-b border-white/5 text-gray-300">
                Rate limiting
              </td>
              <td className="p-3 border-b border-white/5 text-gray-300">
                Too many requests
              </td>
              <td className="p-3 border-b border-white/5 text-gray-300">
                Use caching, batch requests
              </td>
            </tr>
          </tbody>
        </table>
      </div>
    </div>
  </div>
);


