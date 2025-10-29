import React, { useState } from "react";
import { useNavigate } from "react-router-dom";
import { Header } from "../components/Header/Header";
import { MarketDataSection } from "../components/MarketData/MarketDataSection";
import { PortfolioSection } from "../components/Portfolio/PortfolioSection";
import { RiskAnalysisSection } from "../components/RiskAnalysis/RiskAnalysisSection";
import { GlassButton } from "../components/ui/GlassButton";
import { useApiHealth } from "../hooks/useApiHealth";
import { usePortfolio } from "../hooks/usePortfolio";
import { calculateRisk } from "../services/api.service";
import type { RiskMetrics } from "../types";

export const Dashboard: React.FC = () => {
  const navigate = useNavigate();
  const { isOnline: isApiOnline, isChecking, healthData } = useApiHealth();
  const {
    portfolio,
    marketData,
    addInstrument,
    removeInstrument,
    updateMarketData,
  } = usePortfolio();

  const [results, setResults] = useState<RiskMetrics | null>(null);
  const [loading, setLoading] = useState(false);
  const [error, setError] = useState("");

  const handleCalculateRisk = async () => {
    if (portfolio.length === 0) {
      setError("Portfolio is empty. Please add instruments first.");
      return;
    }

    // Filter out market data entries that are empty/undefined
    // Only send data that has been fetched from the backend
    const filteredMarketData: typeof marketData = {};
    Object.entries(marketData).forEach(([assetId, data]) => {
      if (data && data.spot > 0) {
        filteredMarketData[assetId] = data;
      }
    });

    setLoading(true);
    setError("");
    setResults(null);

    try {
      const riskMetrics = await calculateRisk(portfolio, filteredMarketData);
      setResults(riskMetrics);
    } catch (err) {
      setError(
        err instanceof Error ? err.message : "Failed to calculate risk metrics"
      );
    } finally {
      setLoading(false);
    }
  };

  return (
    <div className="min-h-screen">
      <GlassButton
        onClick={() => navigate("/documentation")}
        icon={
          <svg
            className="w-5 h-5"
            fill="none"
            stroke="currentColor"
            viewBox="0 0 24 24"
          >
            <path
              strokeLinecap="round"
              strokeLinejoin="round"
              strokeWidth="2"
              d="M9 12h6m-6 4h6m2 5H7a2 2 0 01-2-2V5a2 2 0 012-2h5.586a1 1 0 01.707.293l5.414 5.414a1 1 0 01.293.707V19a2 2 0 01-2 2z"
            />
          </svg>
        }
        label="Documentation"
        mobileLabel="View Docs"
        title="View Documentation"
      />

      <div className="container mx-auto px-4 py-8 max-w-7xl">
        <Header
          isApiOnline={isApiOnline}
          cachedAssets={healthData?.cache_info?.cached_assets}
        />

        <div className="space-y-8">
          {/* Grid layout for Market Data (30%) and Portfolio Builder (70%) */}
          <div className="grid grid-cols-1 lg:grid-cols-10 gap-8">
            <div className="lg:col-span-3 flex">
              <div className="w-full flex flex-col">
                <MarketDataSection
                  portfolio={portfolio}
                  marketData={marketData}
                  onUpdateMarketData={updateMarketData}
                />
              </div>
            </div>

            <div className="lg:col-span-7 flex">
              <div className="w-full flex flex-col">
                <PortfolioSection
                  portfolio={portfolio}
                  onAddInstrument={(onAddInstrument) =>
                    addInstrument(onAddInstrument)
                  }
                  onRemoveInstrument={removeInstrument}
                  onError={setError}
                />
              </div>
            </div>
          </div>

          <RiskAnalysisSection
            results={results}
            loading={loading}
            error={error}
            onCalculate={handleCalculateRisk}
            onClearError={() => setError("")}
            portfolioSize={portfolio.length}
          />
        </div>

        <footer className="text-center mt-12 text-gray-500 opacity-70 text-sm">
          <p>
            Powered by C++ Quantitative Risk Engine with Python Flask API &amp;
            React + TypeScript
          </p>
        </footer>
      </div>
    </div>
  );
};
