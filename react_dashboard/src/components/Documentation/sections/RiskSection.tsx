import React from "react";

export const RiskSection: React.FC = () => (
  <div className="space-y-6">
    <h2 className="text-3xl font-bold text-cyan-400 mb-6">Risk Metrics</h2>

    <div className="mb-8">
      <h3 className="text-xl font-bold text-gray-100 mb-4">Greeks</h3>
      <p className="text-gray-300 mb-4">
        First and second-order derivatives measuring option price sensitivity to
        various factors.
      </p>

      <div className="space-y-4">
        <div className="bg-gray-800/40 p-5 rounded-lg border border-gray-700/60">
          <h4 className="text-cyan-400 font-bold mb-2">Delta (Δ)</h4>
          <p className="text-sm text-gray-300 mb-2">
            Rate of change of option price with respect to underlying asset
            price
          </p>
          <p className="text-xs text-gray-400">
            Range: 0 to 1 (calls), -1 to 0 (puts)
          </p>
          <p className="text-xs text-gray-400">
            Use: Hedge ratio, directional exposure
          </p>
        </div>

        <div className="bg-gray-800/40 p-5 rounded-lg border border-gray-700/60">
          <h4 className="text-cyan-400 font-bold mb-2">Gamma (Γ)</h4>
          <p className="text-sm text-gray-300 mb-2">
            Rate of change of delta with respect to underlying asset price
          </p>
          <p className="text-xs text-gray-400">Range: Always positive</p>
          <p className="text-xs text-gray-400">
            Use: Delta hedging stability, convexity
          </p>
        </div>

        <div className="bg-gray-800/40 p-5 rounded-lg border border-gray-700/60">
          <h4 className="text-cyan-400 font-bold mb-2">Vega (ν)</h4>
          <p className="text-sm text-gray-300 mb-2">
            Rate of change of option price with respect to volatility
          </p>
          <p className="text-xs text-gray-400">Range: Always positive</p>
          <p className="text-xs text-gray-400">
            Use: Volatility exposure, vega hedging
          </p>
        </div>

        <div className="bg-gray-800/40 p-5 rounded-lg border border-gray-700/60">
          <h4 className="text-cyan-400 font-bold mb-2">Theta (Θ)</h4>
          <p className="text-sm text-gray-300 mb-2">
            Rate of change of option price with respect to time
          </p>
          <p className="text-xs text-gray-400">Range: Usually negative</p>
          <p className="text-xs text-gray-400">Use: Time decay, carry costs</p>
        </div>

        <div className="bg-gray-800/40 p-5 rounded-lg border border-gray-700/60">
          <h4 className="text-cyan-400 font-bold mb-2">Rho (ρ)</h4>
          <p className="text-sm text-gray-300 mb-2">
            Rate of change of option price with respect to interest rate
          </p>
          <p className="text-xs text-gray-400">
            Range: Positive for calls, negative for puts
          </p>
          <p className="text-xs text-gray-400">
            Use: Interest rate sensitivity
          </p>
        </div>
      </div>
    </div>

    <div className="mb-8">
      <h3 className="text-xl font-bold text-gray-100 mb-4">
        Value at Risk (VaR)
      </h3>
      <p className="text-gray-300 mb-4">
        Statistical measure of potential loss over a specified time horizon at a
        given confidence level.
      </p>

      <h4 className="text-sm font-semibold text-gray-300 mb-2">
        Monte Carlo Simulation
      </h4>
      <p className="text-sm text-gray-400 mb-4">
        Generate thousands of random price paths to estimate portfolio value
        distribution.
      </p>

      <div>
        <h4 className="text-sm font-semibold text-gray-300 mb-2">
          Configuration Parameters
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
                Default
              </th>
              <th className="bg-gray-900/90 p-3 text-left font-semibold text-cyan-400 border-b-2 border-cyan-400/30">
                Description
              </th>
            </tr>
          </thead>
          <tbody>
            <tr className="hover:bg-cyan-500/5 transition-colors">
              <td className="p-3 border-b border-white/5 text-gray-300">
                <code>simulations</code>
              </td>
              <td className="p-3 border-b border-white/5 text-gray-300">
                100,000
              </td>
              <td className="p-3 border-b border-white/5 text-gray-300">
                Number of Monte Carlo paths
              </td>
            </tr>
            <tr className="hover:bg-cyan-500/5 transition-colors">
              <td className="p-3 border-b border-white/5 text-gray-300">
                <code>confidence</code>
              </td>
              <td className="p-3 border-b border-white/5 text-gray-300">
                0.95
              </td>
              <td className="p-3 border-b border-white/5 text-gray-300">
                Confidence level (95% or 99%)
              </td>
            </tr>
            <tr className="hover:bg-cyan-500/5 transition-colors">
              <td className="p-3 border-b border-white/5 text-gray-300">
                <code>time_horizon</code>
              </td>
              <td className="p-3 border-b border-white/5 text-gray-300">1.0</td>
              <td className="p-3 border-b border-white/5 text-gray-300">
                Time horizon in days
              </td>
            </tr>
            <tr className="hover:bg-cyan-500/5 transition-colors">
              <td className="p-3 border-b border-white/5 text-gray-300">
                <code>seed</code>
              </td>
              <td className="p-3 border-b border-white/5 text-gray-300">
                random
              </td>
              <td className="p-3 border-b border-white/5 text-gray-300">
                Random seed for reproducibility
              </td>
            </tr>
          </tbody>
        </table>
      </div>

      <div>
        <h4 className="text-sm font-semibold text-gray-300 mb-2">
          Accuracy vs Performance
        </h4>
        <table
          className="w-full border-separate"
          style={{ borderSpacing: 0, margin: "1.5rem 0" }}
        >
          <thead>
            <tr>
              <th className="bg-gray-900/90 p-3 text-left font-semibold text-cyan-400 border-b-2 border-cyan-400/30">
                Simulations
              </th>
              <th className="bg-gray-900/90 p-3 text-left font-semibold text-cyan-400 border-b-2 border-cyan-400/30">
                Standard Error
              </th>
              <th className="bg-gray-900/90 p-3 text-left font-semibold text-cyan-400 border-b-2 border-cyan-400/30">
                Time
              </th>
            </tr>
          </thead>
          <tbody>
            <tr className="hover:bg-cyan-500/5 transition-colors">
              <td className="p-3 border-b border-white/5 text-gray-300">
                10,000
              </td>
              <td className="p-3 border-b border-white/5 text-gray-300">~1%</td>
              <td className="p-3 border-b border-white/5 text-gray-300">
                &lt; 100ms
              </td>
            </tr>
            <tr className="hover:bg-cyan-500/5 transition-colors">
              <td className="p-3 border-b border-white/5 text-gray-300">
                100,000
              </td>
              <td className="p-3 border-b border-white/5 text-gray-300">
                ~0.3%
              </td>
              <td className="p-3 border-b border-white/5 text-gray-300">~1s</td>
            </tr>
            <tr className="hover:bg-cyan-500/5 transition-colors">
              <td className="p-3 border-b border-white/5 text-gray-300">
                1,000,000
              </td>
              <td className="p-3 border-b border-white/5 text-gray-300">
                ~0.1%
              </td>
              <td className="p-3 border-b border-white/5 text-gray-300">
                ~10s
              </td>
            </tr>
          </tbody>
        </table>
      </div>
    </div>

    <div>
      <h3 className="text-xl font-bold text-gray-100 mb-4">
        Expected Shortfall (CVaR)
      </h3>
      <p className="text-gray-300 mb-4">
        Average loss beyond the VaR threshold. More conservative than VaR as it
        considers tail risk.
      </p>

      <div className="bg-yellow-900/20 border border-yellow-500/50 rounded-lg p-4">
        <p className="text-yellow-300 font-semibold mb-2">Interpretation</p>
        <p className="text-sm text-yellow-200">
          If 95% VaR is $10,000, and 95% CVaR is $15,000, this means: In the
          worst 5% of cases, the average loss is $15,000.
        </p>
      </div>
    </div>
  </div>
);


