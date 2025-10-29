import React from "react";
import { StatusIndicator } from "../ui/StatusIndicator";
import { API_CONFIG } from "@/config/constants";

interface HeaderProps {
  isApiOnline: boolean;
  cachedAssets?: number;
}

export const Header: React.FC<HeaderProps> = ({
  isApiOnline,
  cachedAssets,
}) => {
  const statusText = isApiOnline
    ? `API Connected${
        cachedAssets !== undefined
          ? ` (${cachedAssets} assets cached)`
          : "not found"
      }`
    : "API Offline";

  return (
    <header className="text-center mb-8 md:mb-10 fade-in">
      <div className="flex items-center justify-center gap-3 mb-3">
        <h1 className="text-3xl md:text-4xl lg:text-5xl font-extrabold gradient-text">
          {API_CONFIG.NAME}
        </h1>
      </div>
      <h2 className="text-sm md:text-base lg:text-lg font-light text-gray-400 mb-2">
        Community Risk Engine v{API_CONFIG.VERSION}
        <span className="live-badge ml-2">LIVE</span>
      </h2>
      <p className="text-xs text-gray-500 mb-3">
        Market data automatically fetched from Yahoo Finance
      </p>
      <div className="flex items-center justify-center text-xs">
        <StatusIndicator isOnline={isApiOnline} text={statusText} />
      </div>
    </header>
  );
};
