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
  const { isOnline: isApiOnline, isChecking } = useApiHealth();
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

    const uniqueAssets = new Set(portfolio.map((inst) => inst.asset_id));
    const missingMarketData = Array.from(uniqueAssets).filter(
      (asset) =>
        !marketData[asset] ||
        marketData[asset].spot === 0 ||
        marketData[asset].volatility === 0
    );

    if (missingMarketData.length > 0) {
      setError(
        `Missing or incomplete market data for: ${missingMarketData.join(
          ", "
        )}. Please provide all market data.`
      );
      return;
    }

    setLoading(true);
    setError("");
    setResults(null);

    try {
      const riskMetrics = await calculateRisk(portfolio, marketData);
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
    <div className="min-h-screen bg-gradient-to-br from-gray-900 via-blue-900 to-purple-900">
      <div className="container mx-auto px-4 py-8">
        <Header isApiOnline={isApiOnline} />

        <div className="space-y-8">
          {/* Grid layout for Market Data (30%) and Portfolio Builder (70%) */}
          <div className="grid grid-cols-1 lg:grid-cols-10 gap-8">
            <div className="lg:col-span-3">
              <MarketDataSection
                portfolio={portfolio}
                marketData={marketData}
                onUpdateMarketData={updateMarketData}
              />
            </div>

            <div className="lg:col-span-7">
              <PortfolioSection
                portfolio={portfolio}
                onAddInstrument={addInstrument}
                onRemoveInstrument={removeInstrument}
                onError={setError}
              />
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

        <footer className="text-center mt-12 text-gray-500 text-sm">
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
