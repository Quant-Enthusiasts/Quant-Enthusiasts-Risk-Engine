import React, { useState } from "react";
import { Header } from "./components/Header/Header";
import { MarketDataSection } from "./components/MarketData/MarketDataSection";
import { PortfolioSection } from "./components/Portfolio/PortfolioSection";
import { RiskAnalysisSection } from "./components/RiskAnalysis/RiskAnalysisSection";
import { useApiHealth } from "./hooks/useApiHealth";
import { usePortfolio } from "./hooks/usePortfolio";
import { calculateRisk } from "./services/api.service";
import type { RiskMetrics } from "./types";

function App() {
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
}

export default App;
