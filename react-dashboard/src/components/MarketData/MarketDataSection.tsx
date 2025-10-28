import React, { useEffect } from "react";
import type { MarketDataMap } from "@/types";
import { config } from "@/config/constants";
import { getUniqueAssets } from "@/utils/helpers";

interface MarketDataSectionProps {
  portfolio: Array<{ asset_id: string }>;
  marketData: MarketDataMap;
  onUpdateMarketData: (
    assetId: string,
    data: Partial<MarketDataMap[string]>
  ) => void;
}

export const MarketDataSection: React.FC<MarketDataSectionProps> = ({
  portfolio,
  marketData,
  onUpdateMarketData,
}) => {
  const neededAssets =
    portfolio.length > 0 ? getUniqueAssets(portfolio) : new Set(["AAPL"]);

  useEffect(() => {
    // Initialize market data for any missing assets
    neededAssets.forEach((assetId) => {
      if (!marketData[assetId]) {
        onUpdateMarketData(assetId, config.defaultMarketData);
      }
    });
  }, [neededAssets, marketData, onUpdateMarketData]);

  const handleInputChange = (
    assetId: string,
    field: keyof MarketDataMap[string],
    value: string
  ) => {
    const numValue = parseFloat(value);
    if (!isNaN(numValue)) {
      onUpdateMarketData(assetId, { [field]: numValue });
    }
  };

  return (
    <section className="glass-effect rounded-xl p-6 shadow-2xl fade-in">
      <h3 className="text-xl font-semibold mb-4 flex items-center gap-2">
        <span className="text-2xl">📊</span>
        <span className="text-transparent bg-clip-text bg-gradient-to-r from-cyan-400 to-blue-400">
          Market Data
        </span>
      </h3>
      <div className="space-y-4">
        {Array.from(neededAssets).map((assetId) => {
          const data = marketData[assetId] || config.defaultMarketData;
          return (
            <div
              key={assetId}
              className="p-4 bg-gray-800/40 rounded-xl border border-gray-700/60 hover:border-cyan-500/50 transition fade-in"
            >
              <h4 className="text-lg font-bold text-cyan-400 mb-3 flex items-center gap-2">
                <span>📌</span>
                <span>{assetId}</span>
              </h4>
              <div className="grid grid-cols-2 gap-3">
                <div>
                  <label className="text-xs text-gray-400 block mb-1">
                    Spot Price
                  </label>
                  <input
                    type="number"
                    value={data.spot}
                    onChange={(e) =>
                      handleInputChange(assetId, "spot", e.target.value)
                    }
                    step="0.01"
                    min="0.01"
                    className="w-full bg-gray-700/60 text-white p-2 rounded-lg text-sm border border-gray-600 focus:ring-2 focus:ring-cyan-500 focus:border-transparent"
                  />
                </div>
                <div>
                  <label className="text-xs text-gray-400 block mb-1">
                    Rate (r)
                  </label>
                  <input
                    type="number"
                    value={data.rate}
                    onChange={(e) =>
                      handleInputChange(assetId, "rate", e.target.value)
                    }
                    step="0.001"
                    className="w-full bg-gray-700/60 text-white p-2 rounded-lg text-sm border border-gray-600 focus:ring-2 focus:ring-cyan-500 focus:border-transparent"
                  />
                </div>
                <div>
                  <label className="text-xs text-gray-400 block mb-1">
                    Volatility (σ)
                  </label>
                  <input
                    type="number"
                    value={data.vol}
                    onChange={(e) =>
                      handleInputChange(assetId, "vol", e.target.value)
                    }
                    step="0.01"
                    min="0"
                    className="w-full bg-gray-700/60 text-white p-2 rounded-lg text-sm border border-gray-600 focus:ring-2 focus:ring-cyan-500 focus:border-transparent"
                  />
                </div>
                <div>
                  <label className="text-xs text-gray-400 block mb-1">
                    Dividend
                  </label>
                  <input
                    type="number"
                    value={data.dividend}
                    onChange={(e) =>
                      handleInputChange(assetId, "dividend", e.target.value)
                    }
                    step="0.001"
                    min="0"
                    className="w-full bg-gray-700/60 text-white p-2 rounded-lg text-sm border border-gray-600 focus:ring-2 focus:ring-cyan-500 focus:border-transparent"
                  />
                </div>
              </div>
            </div>
          );
        })}
      </div>
    </section>
  );
};
