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
      <div className="flex items-center justify-between mb-6">
        <h2 className="text-2xl font-bold flex items-center gap-2">
          <span className="text-2xl">🎯</span>
          <span className="text-transparent bg-clip-text bg-gradient-to-r from-cyan-400 to-blue-400">
            Risk Analysis
          </span>
        </h2>

        <button
          onClick={onCalculate}
          disabled={!canCalculate}
          className={`font-semibold py-3 px-8 rounded-lg shadow-lg transform active:scale-95 transition-all ${
            canCalculate
              ? "bg-gradient-to-r from-green-600 to-emerald-600 hover:from-green-500 hover:to-emerald-500 text-white hover:shadow-green-500/50 hover:-translate-y-0.5"
              : "bg-gray-700 text-gray-500 cursor-not-allowed"
          }`}
        >
          {loading ? "⏳ Calculating..." : "🚀 Calculate Portfolio Risk"}
        </button>
      </div>

      {!canCalculate && !loading && (
        <p className="text-sm text-gray-400 mb-4 text-center">
          Add instruments and market data to calculate
        </p>
      )}

      {error && (
        <div className="mb-6">
          <ErrorMessage message={error} onClose={onClearError} />
        </div>
      )}

      {loading && (
        <div className="py-12">
          <Spinner message="Calculating risk metrics..." />
        </div>
      )}

      {!loading && results && (
        <div className="space-y-6 fade-in">
          {/* Main Metrics - Horizontal Cards */}
          <div className="grid grid-cols-2 lg:grid-cols-6 gap-4">
            <div className="bg-gray-800/50 rounded-lg p-5 border border-gray-700 hover:border-green-500/50 transition-all">
              <div className="text-xs text-gray-400 uppercase tracking-wide mb-2">
                Total PV
              </div>
              <div className="text-2xl font-bold text-green-400">
                $
                {results.total_pv.toLocaleString(undefined, {
                  minimumFractionDigits: 2,
                  maximumFractionDigits: 2,
                })}
              </div>
            </div>

            <div className="bg-gray-800/50 rounded-lg p-5 border border-gray-700 hover:border-yellow-500/50 transition-all">
              <div className="text-xs text-gray-400 uppercase tracking-wide mb-2">
                Delta
              </div>
              <div className="text-2xl font-bold text-yellow-400">
                {results.total_delta.toFixed(2)}
              </div>
            </div>

            <div className="bg-gray-800/50 rounded-lg p-5 border border-gray-700 hover:border-cyan-500/50 transition-all">
              <div className="text-xs text-gray-400 uppercase tracking-wide mb-2">
                Gamma
              </div>
              <div className="text-2xl font-bold text-cyan-400">
                {results.total_gamma.toFixed(2)}
              </div>
            </div>

            <div className="bg-gray-800/50 rounded-lg p-5 border border-gray-700 hover:border-cyan-500/50 transition-all">
              <div className="text-xs text-gray-400 uppercase tracking-wide mb-2">
                Vega
              </div>
              <div className="text-2xl font-bold text-cyan-400">
                {results.total_vega.toLocaleString(undefined, {
                  minimumFractionDigits: 2,
                  maximumFractionDigits: 2,
                })}
              </div>
            </div>

            <div className="bg-gray-800/50 rounded-lg p-5 border border-gray-700 hover:border-cyan-500/50 transition-all">
              <div className="text-xs text-gray-400 uppercase tracking-wide mb-2">
                Theta
              </div>
              <div className="text-2xl font-bold text-cyan-400">
                {results.total_theta.toFixed(2)}
              </div>
            </div>

            <div className="bg-gray-800/50 rounded-lg p-5 border border-gray-700 hover:border-red-500/50 transition-all">
              <div className="text-xs text-gray-400 uppercase tracking-wide mb-2">
                95% VaR (1D)
              </div>
              <div className="text-2xl font-bold text-red-400">
                ${Math.abs(results.value_at_risk_95).toFixed(2)}
              </div>
            </div>
          </div>
        </div>
      )}

      {!loading && !results && !error && (
        <div className="text-center py-12 text-gray-400">
          <p className="text-lg">📊 No results yet</p>
          <p className="text-sm mt-2">
            Click "Calculate Portfolio Risk" to begin analysis
          </p>
        </div>
      )}
    </section>
  );
};
