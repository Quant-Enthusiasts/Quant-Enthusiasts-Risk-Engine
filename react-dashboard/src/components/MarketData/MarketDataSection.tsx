import React, { useState, useEffect } from "react";
import type { MarketDataMap } from "@/types";
import { getUniqueAssets } from "@/utils/helpers";
import { config } from "@/config/constants";

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
  const [isRefreshing, setIsRefreshing] = useState(false);
  const [lastUpdated, setLastUpdated] = useState<Record<string, Date>>({});
  const [fetchedAssets, setFetchedAssets] = useState<Set<string>>(new Set());
  const neededAssets =
    portfolio.length > 0 ? getUniqueAssets(portfolio) : new Set(["AAPL"]);

  // Auto-fetch cached data when new assets are added to portfolio
  useEffect(() => {
    const checkCachedData = async () => {
      // Get assets that haven't been checked yet
      const newAssets = Array.from(neededAssets).filter(
        (asset) => !fetchedAssets.has(asset) && !lastUpdated[asset]
      );

      if (newAssets.length === 0) return;

      try {
        // Try to fetch without force_refresh to get cached data if available
        const response = await fetch(
          `${config.apiBaseUrl}/update_market_data`,
          {
            method: "POST",
            headers: {
              "Content-Type": "application/json",
            },
            body: JSON.stringify({
              tickers: newAssets,
              force_refresh: false,
            }),
          }
        );

        if (response.ok || response.status === 207) {
          const data = await response.json();

          const currentTime = new Date();
          const newLastUpdated: Record<string, Date> = {};
          const newFetchedAssets = new Set(fetchedAssets);

          Object.entries(data.updated || {}).forEach(
            ([assetId, assetData]: [string, any]) => {
              console.log(` Found cached data for ${assetId}`);
              onUpdateMarketData(assetId, {
                spot: assetData.spot,
                vol: assetData.vol,
                rate: assetData.rate,
                dividend: assetData.dividend,
              });
              newLastUpdated[assetId] = currentTime;
              newFetchedAssets.add(assetId);
            }
          );

          if (Object.keys(newLastUpdated).length > 0) {
            setLastUpdated((prev) => ({ ...prev, ...newLastUpdated }));
            setFetchedAssets(newFetchedAssets);
          }
        }
      } catch (error) {
        console.error("Error checking cached data:", error);
      }
    };

    checkCachedData();
  }, [neededAssets, fetchedAssets, lastUpdated, onUpdateMarketData]);

  const handleRefresh = async () => {
    if (neededAssets.size === 0) return;

    setIsRefreshing(true);

    try {
      // Call the API with force_refresh=true
      const assetsArray = Array.from(neededAssets);
      const requestBody = {
        tickers: assetsArray,
        force_refresh: true,
      };

      const response = await fetch(`${config.apiBaseUrl}/update_market_data`, {
        method: "POST",
        headers: {
          "Content-Type": "application/json",
        },
        body: JSON.stringify(requestBody),
      });

      if (response.ok || response.status === 207) {
        const data = await response.json();

        const currentTime = new Date();
        const newLastUpdated: Record<string, Date> = {};
        const newFetchedAssets = new Set(fetchedAssets);

        // Update market data for each asset
        Object.entries(data.updated || {}).forEach(
          ([assetId, assetData]: [string, any]) => {
            onUpdateMarketData(assetId, {
              spot: assetData.spot,
              vol: assetData.vol,
              rate: assetData.rate,
              dividend: assetData.dividend,
            });
            newLastUpdated[assetId] = currentTime;
            newFetchedAssets.add(assetId);
          }
        );

        setLastUpdated((prev) => ({ ...prev, ...newLastUpdated }));
        setFetchedAssets(newFetchedAssets);
      } else {
        console.error("Failed to fetch market data:", response.statusText);
      }
    } catch (error) {
      console.error("Error refreshing market data:", error);
    } finally {
      setIsRefreshing(false);
    }
  };

  const getTimeAgo = (assetId: string): string => {
    const lastUpdate = lastUpdated[assetId];
    if (!lastUpdate) return "";

    const now = new Date();
    const diffMs = now.getTime() - lastUpdate.getTime();
    const diffMins = Math.floor(diffMs / 60000);
    const diffHours = Math.floor(diffMins / 60);

    if (diffHours > 0) return `${diffHours}h ago`;
    if (diffMins > 0) return `${diffMins}m ago`;
    return "Just now";
  };

  return (
    <section className="glass-effect rounded-xl p-6 shadow-2xl fade-in h-full">
      <div className="flex justify-between items-center mb-5">
        <h2 className="text-lg font-bold flex items-center gap-2">
          <span className="section-icon">
            <svg
              className="w-4 h-4 text-cyan-400"
              fill="none"
              stroke="currentColor"
              viewBox="0 0 24 24"
            >
              <path
                strokeLinecap="round"
                strokeLinejoin="round"
                strokeWidth="2"
                d="M9 19v-6a2 2 0 00-2-2H5a2 2 0 00-2 2v6a2 2 0 002 2h2a2 2 0 002-2zm0 0V9a2 2 0 012-2h2a2 2 0 012 2v10m-6 0a2 2 0 002 2h2a2 2 0 002-2m0 0V5a2 2 0 012-2h2a2 2 0 012 2v14a2 2 0 01-2 2h-2a2 2 0 01-2-2z"
              />
            </svg>
          </span>
          <span className="text-white">
            Live Market
            <br />
            Data
          </span>
        </h2>
        <button
          onClick={handleRefresh}
          disabled={isRefreshing}
          className="bg-blue-600 text-white text-xs font-semibold px-3 py-1.5 rounded-md shadow-md hover:bg-blue-500 transition-colors disabled:opacity-50 disabled:cursor-not-allowed"
          title="Refresh market data"
        >
          {isRefreshing ? "⏳ Fetching..." : "🔄 Refresh"}
        </button>
      </div>
      <div className="space-y-3 text-sm">
        {Array.from(neededAssets).map((assetId) => {
          const data = marketData[assetId];
          const timeAgo = getTimeAgo(assetId);
          const hasFetched = fetchedAssets.has(assetId);

          return (
            <div
              key={assetId}
              className="market-data-card p-4 bg-gray-800/40 rounded-xl border border-gray-700/60 hover:border-cyan-500/50 transition fade-in"
            >
              <div className="flex justify-between items-start mb-3">
                <h4 className="text-base font-bold text-cyan-400 flex items-center gap-2">
                  <span>📌</span>
                  <span>{assetId}</span>
                </h4>
                {timeAgo && (
                  <span className="text-xs text-gray-500">{timeAgo}</span>
                )}
              </div>

              {!hasFetched || !data ? (
                <div className="flex items-center gap-2 text-yellow-500 text-sm">
                  <span>⚠️</span>
                  <span>Will fetch on calculate</span>
                </div>
              ) : (
                <div className="grid grid-cols-2 gap-x-4 gap-y-2">
                  <div className="flex justify-between">
                    <span className="text-sm text-gray-400">Spot:</span>
                    <span className="text-sm font-semibold text-white">
                      {data.spot.toFixed(2)}
                    </span>
                  </div>
                  <div className="flex justify-between">
                    <span className="text-sm text-gray-400">Vol:</span>
                    <span className="text-sm font-semibold text-white">
                      {(data.vol * 100).toFixed(1)}%
                    </span>
                  </div>
                  <div className="flex justify-between">
                    <span className="text-sm text-gray-400">Rate:</span>
                    <span className="text-sm font-semibold text-white">
                      {(data.rate * 100).toFixed(2)}%
                    </span>
                  </div>
                  <div className="flex justify-between">
                    <span className="text-sm text-gray-400">Div:</span>
                    <span className="text-sm font-semibold text-white">
                      {(data.dividend * 100).toFixed(2)}%
                    </span>
                  </div>
                </div>
              )}
            </div>
          );
        })}
      </div>
    </section>
  );
};
