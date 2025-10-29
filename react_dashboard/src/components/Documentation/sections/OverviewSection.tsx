import React from "react";

export const OverviewSection: React.FC = () => (
  <div className="space-y-6">
    <h2 className="text-3xl font-bold text-cyan-400 mb-6">Overview</h2>

    <div className="mb-8">
      <p className="text-gray-300 text-lg mb-4">
        A high-performance quantitative finance platform for portfolio risk
        management and options pricing. Built with C++17 core, Python bindings,
        and an interactive web dashboard.
      </p>
    </div>

    <div className="grid grid-cols-1 md:grid-cols-2 gap-4 mb-8">
      <div className="bg-gray-800/40 p-5 rounded-lg border border-gray-700/60 hover:border-cyan-400/40 transition-all duration-300 hover:-translate-y-1">
        <h4 className="text-cyan-400 font-bold mb-2">
          Multiple Pricing Models
        </h4>
        <p className="text-sm text-gray-400">
          Black-Scholes, Binomial Tree, Merton Jump Diffusion
        </p>
      </div>
      <div className="bg-gray-800/40 p-5 rounded-lg border border-gray-700/60 hover:border-cyan-400/40 transition-all duration-300 hover:-translate-y-1">
        <h4 className="text-cyan-400 font-bold mb-2">Risk Analytics</h4>
        <p className="text-sm text-gray-400">
          Greeks calculation, VaR, Portfolio aggregation
        </p>
      </div>
      <div className="bg-gray-800/40 p-5 rounded-lg border border-gray-700/60 hover:border-cyan-400/40 transition-all duration-300 hover:-translate-y-1">
        <h4 className="text-cyan-400 font-bold mb-2">Live Market Data</h4>
        <p className="text-sm text-gray-400">
          Automatic fetching from Yahoo Finance with caching
        </p>
      </div>
      <div className="bg-gray-800/40 p-5 rounded-lg border border-gray-700/60 hover:border-cyan-400/40 transition-all duration-300 hover:-translate-y-1">
        <h4 className="text-cyan-400 font-bold mb-2">RESTful API</h4>
        <p className="text-sm text-gray-400">
          Flask-based endpoints with validation
        </p>
      </div>
    </div>

    <h3 className="text-xl font-bold text-gray-100 mb-4">
      System Architecture
    </h3>
    <div
      className="code-block mb-6"
      style={{ whiteSpace: "pre", textAlign: "left" }}
    >
      {`JavaScript Dashboard (Frontend)
|
| HTTP/JSON
v
Python API Layer (Flask)
|
| pybind11
v
C++ Risk Engine (Core)`}
    </div>

    <h3 className="text-xl font-bold text-gray-100 mb-4">Key Features</h3>
    <ul className="space-y-2 text-gray-300 mb-8">
      <li className="flex items-start gap-2">
        <span className="text-cyan-400 mt-1">•</span>
        <span>European and American options pricing</span>
      </li>
      <li className="flex items-start gap-2">
        <span className="text-cyan-400 mt-1">•</span>
        <span>
          Complete Greeks calculation (Delta, Gamma, Vega, Theta, Rho)
        </span>
      </li>
      <li className="flex items-start gap-2">
        <span className="text-cyan-400 mt-1">•</span>
        <span>Monte Carlo Value at Risk with configurable simulations</span>
      </li>
      <li className="flex items-start gap-2">
        <span className="text-cyan-400 mt-1">•</span>
        <span>Live market data integration with 24-hour caching</span>
      </li>
      <li className="flex items-start gap-2">
        <span className="text-cyan-400 mt-1">•</span>
        <span>Portfolio-level risk aggregation and analytics</span>
      </li>
    </ul>

    <div>
      <h3 className="text-xl font-bold text-gray-100 mb-4">Monitoring</h3>

      <h4 className="text-sm font-semibold text-gray-300 mb-2">
        Health Checks
      </h4>
      <div className="code-block mb-4" style={{ whiteSpace: "pre" }}>
        {`# Kubernetes liveness probe
livenessProbe:
  httpGet:
    path: /health
    port: 5000
  initialDelaySeconds: 30
  periodSeconds: 10`}
      </div>

      <h4 className="text-sm font-semibold text-gray-300 mb-2">
        Key Metrics to Monitor
      </h4>
      <ul className="text-sm text-gray-400 space-y-1">
        <li>• Request rate and latency</li>
        <li>• Error rate by endpoint</li>
        <li>• Cache hit/miss ratio</li>
        <li>• Memory and CPU usage</li>
        <li>• Market data fetch failures</li>
      </ul>
    </div>
  </div>
);
