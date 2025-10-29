import { useState, useCallback } from "react";
import type { Instrument, MarketDataMap } from "@/types";
import { config } from "@/config/constants";
import { getUniqueAssets } from "@/utils/helpers";

export const usePortfolio = () => {
  const [portfolio, setPortfolio] = useState<Instrument[]>([]);
  const [marketData, setMarketData] = useState<MarketDataMap>({});

  const addInstrument = useCallback((instrument: Instrument) => {
    setPortfolio((prev) => [...prev, instrument]);
  }, []);

  const removeInstrument = useCallback((index: number) => {
    setPortfolio((prev) => prev.filter((_, i) => i !== index));
  }, []);

  const updateMarketData = useCallback(
    (assetId: string, data: Partial<MarketDataMap[string]>) => {
      setMarketData((prev) => ({
        ...prev,
        [assetId]: {
          ...config.defaultMarketData,
          ...prev[assetId],
          ...data,
        },
      }));
    },
    []
  );

  const initializeMarketData = useCallback(
    (assetIds: Set<string>) => {
      const newMarketData: MarketDataMap = { ...marketData };

      assetIds.forEach((assetId) => {
        if (!newMarketData[assetId]) {
          newMarketData[assetId] = { ...config.defaultMarketData };
        }
      });

      setMarketData(newMarketData);
    },
    [marketData]
  );

  // Initialize market data when portfolio changes
  const updateMarketDataForPortfolio = useCallback(() => {
    const assets = getUniqueAssets(portfolio);
    initializeMarketData(assets);
  }, [portfolio, initializeMarketData]);

  return {
    portfolio,
    marketData,
    addInstrument,
    removeInstrument,
    updateMarketData,
    updateMarketDataForPortfolio,
  };
};
