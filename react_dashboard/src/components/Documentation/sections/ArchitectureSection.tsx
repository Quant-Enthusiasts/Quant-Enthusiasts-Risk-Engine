import React from "react";

export const ArchitectureSection: React.FC = () => (
  <div className="space-y-6">
    <h2 className="text-3xl font-bold text-cyan-400 mb-6">Architecture</h2>

    <div className="mb-8">
      <h3 className="text-xl font-bold text-gray-100 mb-4">
        System Components
      </h3>

      <div className="space-y-4">
        <div className="bg-gray-800/40 p-5 rounded-lg border border-gray-700/60">
          <h4 className="text-cyan-400 font-bold mb-2">C++ Risk Engine</h4>
          <p className="text-sm text-gray-300 mb-3">
            Core computational layer for high-performance calculations
          </p>
          <ul className="text-sm text-gray-400 space-y-1">
            <li>
              • Option pricing algorithms (Black-Scholes, Binomial, Jump
              Diffusion)
            </li>
            <li>• Greeks calculation (Delta, Gamma, Vega, Theta, Rho)</li>
            <li>• Monte Carlo VaR simulation</li>
            <li>• Portfolio aggregation logic</li>
          </ul>
        </div>

        <div className="bg-gray-800/40 p-5 rounded-lg border border-gray-700/60">
          <h4 className="text-cyan-400 font-bold mb-2">Python API Layer</h4>
          <p className="text-sm text-gray-300 mb-3">
            Flask-based REST API with pybind11 bindings
          </p>
          <ul className="text-sm text-gray-400 space-y-1">
            <li>• RESTful endpoint definitions</li>
            <li>• Request validation and error handling</li>
            <li>• Market data fetching from Yahoo Finance</li>
            <li>• SQLite caching layer</li>
          </ul>
        </div>

        <div className="bg-gray-800/40 p-5 rounded-lg border border-gray-700/60">
          <h4 className="text-cyan-400 font-bold mb-2">JavaScript Dashboard</h4>
          <p className="text-sm text-gray-300 mb-3">
            Interactive web interface for portfolio management
          </p>
          <ul className="text-sm text-gray-400 space-y-1">
            <li>• Portfolio builder with live market data</li>
            <li>• Risk metrics visualization</li>
            <li>• Real-time API health monitoring</li>
            <li>• Responsive design with Tailwind CSS</li>
          </ul>
        </div>
      </div>
    </div>

    <div className="mb-8">
      <h3 className="text-xl font-bold text-gray-100 mb-4">Data Flow</h3>
      <div className="code-block" style={{ whiteSpace: "pre" }}>
        {`1. User Action (Dashboard)
   ↓
2. HTTP Request (JSON)
   ↓
3. Flask Endpoint Handler
   ↓
4. Market Data Check (Cache/YFinance)
   ↓
5. Python-to-C++ Bridge (pybind11)
   ↓
6. C++ Risk Calculation
   ↓
7. Results Returned (JSON)
   ↓
8. Dashboard Update`}
      </div>
    </div>

    <div>
      <h3 className="text-xl font-bold text-gray-100 mb-4">
        Performance Characteristics
      </h3>
      <div className="w-full overflow-x-auto">
        <table
          className="w-full border-separate"
          style={{ borderSpacing: 0, margin: "1.5rem 0" }}
        >
          <thead>
            <tr>
              <th className="bg-gray-900/90 p-3 text-left font-semibold text-cyan-400 border-b-2 border-cyan-400/30">
                Operation
              </th>
              <th className="bg-gray-900/90 p-3 text-left font-semibold text-cyan-400 border-b-2 border-cyan-400/30">
                Latency
              </th>
              <th className="bg-gray-900/90 p-3 text-left font-semibold text-cyan-400 border-b-2 border-cyan-400/30">
                Notes
              </th>
            </tr>
          </thead>
          <tbody>
            <tr className="hover:bg-cyan-500/5 transition-colors">
              <td className="p-3 border-b border-white/5 text-gray-300">
                Single option pricing
              </td>
              <td className="p-3 border-b border-white/5 text-gray-300">
                &lt; 1 microsecond
              </td>
              <td className="p-3 border-b border-white/5 text-gray-300">
                Black-Scholes analytical
              </td>
            </tr>
            <tr className="hover:bg-cyan-500/5 transition-colors">
              <td className="p-3 border-b border-white/5 text-gray-300">
                Portfolio (100 options)
              </td>
              <td className="p-3 border-b border-white/5 text-gray-300">
                &lt; 1 millisecond
              </td>
              <td className="p-3 border-b border-white/5 text-gray-300">
                Full Greeks calculation
              </td>
            </tr>
            <tr className="hover:bg-cyan-500/5 transition-colors">
              <td className="p-3 border-b border-white/5 text-gray-300">
                VaR (100K simulations)
              </td>
              <td className="p-3 border-b border-white/5 text-gray-300">
                &lt; 1 second
              </td>
              <td className="p-3 border-b border-white/5 text-gray-300">
                Monte Carlo 95% confidence
              </td>
            </tr>
            <tr className="hover:bg-cyan-500/5 transition-colors">
              <td className="p-3 border-b border-white/5 text-gray-300">
                Market data fetch
              </td>
              <td className="p-3 border-b border-white/5 text-gray-300">
                1-3 seconds
              </td>
              <td className="p-3 border-b border-white/5 text-gray-300">
                Yahoo Finance API call
              </td>
            </tr>
          </tbody>
        </table>
      </div>
    </div>
  </div>
);


