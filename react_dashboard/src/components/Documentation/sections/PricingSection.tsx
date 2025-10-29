import React from "react";

export const PricingSection: React.FC = () => (
  <div className="space-y-6">
    <h2 className="text-3xl font-bold text-cyan-400 mb-6">Pricing Models</h2>

    <div className="mb-8">
      <h3 className="text-xl font-bold text-gray-100 mb-4">
        Black-Scholes Model
      </h3>
      <span className="inline-block px-3 py-1 rounded-full text-xs font-semibold uppercase tracking-wide bg-blue-500/20 text-blue-400 mb-4 mr-2">
        Analytical
      </span>
      <span className="inline-block px-3 py-1 rounded-full text-xs font-semibold uppercase tracking-wide bg-green-500/20 text-green-400 mb-4">
        European Only
      </span>

      <p className="text-gray-300 mb-4">
        Analytical solution for European options pricing. Fastest method with
        exact Greeks.
      </p>

      <h4 className="text-sm font-semibold text-gray-300 mb-2">
        Key Assumptions
      </h4>
      <ul className="text-sm text-gray-400 space-y-1 mb-4">
        <li>• Constant volatility</li>
        <li>• Log-normal price distribution</li>
        <li>• No transaction costs</li>
        <li>• Continuous trading</li>
        <li>• Constant risk-free rate</li>
      </ul>

      <h4 className="text-sm font-semibold text-gray-300 mb-2">
        Usage Example
      </h4>
      <div className="code-block" style={{ whiteSpace: "pre" }}>
        {`{
  "pricing_model": "blackscholes",
  "type": "call",
  "strike": 100,
  "expiry": 1.0,
  "style": "european"
}`}
      </div>
    </div>

    <div className="mb-8">
      <h3 className="text-xl font-bold text-gray-100 mb-4">
        Binomial Tree Model
      </h3>
      <span className="inline-block px-3 py-1 rounded-full text-xs font-semibold uppercase tracking-wide bg-purple-500/20 text-purple-400 mb-4 mr-2">
        Numerical
      </span>
      <span className="inline-block px-3 py-1 rounded-full text-xs font-semibold uppercase tracking-wide bg-green-500/20 text-green-400 mb-4">
        American & European
      </span>

      <p className="text-gray-300 mb-4">
        Discrete-time numerical method that handles early exercise. Converges to
        Black-Scholes as steps increase.
      </p>

      <h4 className="text-sm font-semibold text-gray-300 mb-2">Key Features</h4>
      <ul className="text-sm text-gray-400 space-y-1 mb-4">
        <li>• Handles American-style early exercise</li>
        <li>• Configurable time steps (default: 100)</li>
        <li>• Accuracy improves with more steps</li>
        <li>• Suitable for discrete dividends</li>
      </ul>

      <div>
        <h4 className="text-sm font-semibold text-gray-300 mb-2">
          Accuracy vs Performance
        </h4>
        <table
          className="mb-4 w-full border-separate"
          style={{ borderSpacing: 0, margin: "1.5rem 0" }}
        >
          <thead>
            <tr>
              <th className="bg-gray-900/90 p-3 text-left font-semibold text-cyan-400 border-b-2 border-cyan-400/30">
                Steps
              </th>
              <th className="bg-gray-900/90 p-3 text-left font-semibold text-cyan-400 border-b-2 border-cyan-400/30">
                Error
              </th>
              <th className="bg-gray-900/90 p-3 text-left font-semibold text-cyan-400 border-b-2 border-cyan-400/30">
                Time
              </th>
            </tr>
          </thead>
          <tbody>
            <tr className="hover:bg-cyan-500/5 transition-colors">
              <td className="p-3 border-b border-white/5 text-gray-300">100</td>
              <td className="p-3 border-b border-white/5 text-gray-300">
                ±0.1%
              </td>
              <td className="p-3 border-b border-white/5 text-gray-300">
                &lt; 1ms
              </td>
            </tr>
            <tr className="hover:bg-cyan-500/5 transition-colors">
              <td className="p-3 border-b border-white/5 text-gray-300">
                1000
              </td>
              <td className="p-3 border-b border-white/5 text-gray-300">
                ±0.01%
              </td>
              <td className="p-3 border-b border-white/5 text-gray-300">
                ~5ms
              </td>
            </tr>
            <tr className="hover:bg-cyan-500/5 transition-colors">
              <td className="p-3 border-b border-white/5 text-gray-300">
                10000
              </td>
              <td className="p-3 border-b border-white/5 text-gray-300">
                ±0.001%
              </td>
              <td className="p-3 border-b border-white/5 text-gray-300">
                ~50ms
              </td>
            </tr>
          </tbody>
        </table>
      </div>

      <h4 className="text-sm font-semibold text-gray-300 mb-2">
        Usage Example
      </h4>
      <div className="code-block" style={{ whiteSpace: "pre" }}>
        {`{
  "pricing_model": "binomial",
  "binomial_steps": 1000,
  "type": "put",
  "strike": 95,
  "expiry": 0.5,
  "style": "american"
}`}
      </div>
    </div>

    <div>
      <h3 className="text-xl font-bold text-gray-100 mb-4">
        Merton Jump Diffusion Model
      </h3>
      <span className="inline-block px-3 py-1 rounded-full text-xs font-semibold uppercase tracking-wide bg-red-500/20 text-red-400 mb-4 mr-2">
        Advanced
      </span>
      <span className="inline-block px-3 py-1 rounded-full text-xs font-semibold uppercase tracking-wide bg-green-500/20 text-green-400 mb-4">
        European Only
      </span>

      <p className="text-gray-300 mb-4">
        Extension of Black-Scholes that incorporates discontinuous price jumps
        for modeling extreme events.
      </p>

      <div>
        <h4 className="text-sm font-semibold text-gray-300 mb-2">
          Jump Parameters
        </h4>
        <table
          className="mb-4 w-full border-separate"
          style={{ borderSpacing: 0, margin: "1.5rem 0" }}
        >
          <thead>
            <tr>
              <th className="bg-gray-900/90 p-3 text-left font-semibold text-cyan-400 border-b-2 border-cyan-400/30">
                Parameter
              </th>
              <th className="bg-gray-900/90 p-3 text-left font-semibold text-cyan-400 border-b-2 border-cyan-400/30">
                Description
              </th>
              <th className="bg-gray-900/90 p-3 text-left font-semibold text-cyan-400 border-b-2 border-cyan-400/30">
                Typical Range
              </th>
            </tr>
          </thead>
          <tbody>
            <tr className="hover:bg-cyan-500/5 transition-colors">
              <td className="p-3 border-b border-white/5 text-gray-300">
                <code>lambda</code>
              </td>
              <td className="p-3 border-b border-white/5 text-gray-300">
                Jump intensity (jumps per year)
              </td>
              <td className="p-3 border-b border-white/5 text-gray-300">
                0.5 - 5.0
              </td>
            </tr>
            <tr className="hover:bg-cyan-500/5 transition-colors">
              <td className="p-3 border-b border-white/5 text-gray-300">
                <code>mean</code>
              </td>
              <td className="p-3 border-b border-white/5 text-gray-300">
                Mean jump size
              </td>
              <td className="p-3 border-b border-white/5 text-gray-300">
                -0.2 to 0.2
              </td>
            </tr>
            <tr className="hover:bg-cyan-500/5 transition-colors">
              <td className="p-3 border-b border-white/5 text-gray-300">
                <code>vol</code>
              </td>
              <td className="p-3 border-b border-white/5 text-gray-300">
                Jump size volatility
              </td>
              <td className="p-3 border-b border-white/5 text-gray-300">
                0.05 - 0.3
              </td>
            </tr>
          </tbody>
        </table>
      </div>

      <h4 className="text-sm font-semibold text-gray-300 mb-2">
        Usage Example
      </h4>
      <div className="code-block" style={{ whiteSpace: "pre" }}>
        {`{
  "pricing_model": "jumpdiffusion",
  "jump_parameters": {
    "lambda": 2.0,
    "mean": -0.05,
    "vol": 0.15
  },
  "type": "call",
  "strike": 100,
  "expiry": 1.0,
  "style": "european"
}`}
      </div>
    </div>
  </div>
);


