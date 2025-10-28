import React from "react";
import type { RiskMetrics } from "@/types";
import { MetricCard } from "./MetricCard";
import { Spinner } from "../ui/Spinner";
import { ErrorMessage } from "../ui/ErrorMessage";

interface RiskAnalysisSectionProps {
  results: RiskMetrics | null;
  loading: boolean;
  error: string;
  onCalculate: () => void;
  onClearError: () => void;
  portfolioSize: number;
}

export const RiskAnalysisSection: React.FC<RiskAnalysisSectionProps> = ({
  results,
  loading,
  error,
  onCalculate,
  onClearError,
  portfolioSize,
}) => {
  const canCalculate = portfolioSize > 0 && !loading;

  return (
    <section className="glass-effect rounded-xl p-6 fade-in">
      <h2 className="text-2xl font-bold mb-6 flex items-center gap-2">
        <span className="text-2xl">🎯</span>
        <span className="text-transparent bg-clip-text bg-gradient-to-r from-cyan-400 to-blue-400">
          Risk Analysis
        </span>
      </h2>

      <div className="mb-6">
        <button
          onClick={onCalculate}
          disabled={!canCalculate}
          className={`w-full font-semibold py-4 px-6 rounded-lg shadow-lg transform active:scale-95 transition-all ${
            canCalculate
              ? "bg-gradient-to-r from-purple-600 to-pink-600 hover:from-purple-500 hover:to-pink-500 text-white hover:shadow-purple-500/50 hover:-translate-y-0.5"
              : "bg-gray-700 text-gray-500 cursor-not-allowed"
          }`}
        >
          {loading ? "⏳ Calculating..." : "🚀 Calculate Risk Metrics"}
        </button>
        {!canCalculate && !loading && (
          <p className="text-sm text-gray-400 mt-2 text-center">
            Add instruments and market data to calculate
          </p>
        )}
      </div>

      {error && <ErrorMessage message={error} onClose={onClearError} />}

      {loading && (
        <div className="py-12">
          <Spinner message="Calculating risk metrics..." />
        </div>
      )}

      {!loading && results && (
        <div className="space-y-6 fade-in">
          <div>
            <h3 className="text-lg font-semibold mb-4 text-gray-200">
              Portfolio Value
            </h3>
            <div className="grid grid-cols-1 sm:grid-cols-2 gap-4">
              <MetricCard label="Total PV" value={results.total_pv} />
              <MetricCard
                label="Portfolio Size"
                value={results.portfolio_size.toString()}
              />
            </div>
          </div>

          <div>
            <h3 className="text-lg font-semibold mb-4 text-gray-200">Greeks</h3>
            <div className="grid grid-cols-1 sm:grid-cols-2 lg:grid-cols-3 gap-4">
              <MetricCard label="Delta (Δ)" value={results.total_delta} />
              <MetricCard label="Gamma (Γ)" value={results.total_gamma} />
              <MetricCard label="Vega (ν)" value={results.total_vega} />
              <MetricCard label="Theta (Θ)" value={results.total_theta} />
            </div>
          </div>

          <div>
            <h3 className="text-lg font-semibold mb-4 text-gray-200">
              Risk Metrics
            </h3>
            <div className="grid grid-cols-1 gap-4">
              <MetricCard
                label="Value at Risk (95%)"
                value={results.value_at_risk_95}
                isRisk
              />
            </div>
          </div>

          {results.var_parameters && (
            <div className="bg-gradient-to-r from-cyan-500/10 to-blue-500/10 border border-cyan-500/30 rounded-lg p-4">
              <div className="flex items-start">
                <span className="text-2xl mr-3">⚙️</span>
                <div className="text-sm text-gray-300">
                  <p className="font-semibold mb-1">VaR Parameters:</p>
                  <ul className="list-disc list-inside space-y-1">
                    <li>
                      Simulations:{" "}
                      {results.var_parameters.simulations.toLocaleString()}
                    </li>
                    <li>
                      Confidence Level:{" "}
                      {(results.var_parameters.confidence_level * 100).toFixed(
                        1
                      )}
                      %
                    </li>
                    <li>
                      Time Horizon: {results.var_parameters.time_horizon_days}{" "}
                      days
                    </li>
                  </ul>
                </div>
              </div>
            </div>
          )}

          <div className="bg-gradient-to-r from-cyan-500/10 to-blue-500/10 border border-cyan-500/30 rounded-lg p-4">
            <div className="flex items-start">
              <span className="text-2xl mr-3">💡</span>
              <div className="text-sm text-gray-300">
                <p className="font-semibold mb-1">Risk Interpretation:</p>
                <ul className="list-disc list-inside space-y-1">
                  <li>
                    <span className="text-green-400">Positive Delta</span>:
                    Portfolio benefits from price increases
                  </li>
                  <li>
                    <span className="text-yellow-400">High Gamma</span>: Delta
                    sensitivity to price changes
                  </li>
                  <li>
                    <span className="text-red-400">VaR 95%</span>: Maximum
                    expected loss at 95% confidence
                  </li>
                </ul>
              </div>
            </div>
          </div>
        </div>
      )}

      {!loading && !results && !error && (
        <div className="text-center py-12 text-gray-400">
          <p className="text-lg">📊 No results yet</p>
          <p className="text-sm mt-2">
            Click "Calculate Risk Metrics" to begin analysis
          </p>
        </div>
      )}
    </section>
  );
};
