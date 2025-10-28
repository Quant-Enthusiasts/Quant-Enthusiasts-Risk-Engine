import React from "react";
import { StatusIndicator } from "../ui/StatusIndicator";
import { API_CONFIG } from "@/config/constants";

interface HeaderProps {
  isApiOnline: boolean;
}

export const Header: React.FC<HeaderProps> = ({ isApiOnline }) => {
  return (
    <header className="text-center mb-8 fade-in">
      <h1 className="text-5xl md:text-6xl font-bold mb-4 text-transparent bg-clip-text bg-gradient-to-r from-cyan-400 via-blue-400 to-purple-400">
        {API_CONFIG.NAME}
      </h1>
      <p className="text-xl text-gray-400 mb-2">Community Risk Engine</p>
      <p className="text-sm text-gray-500 mb-4">Version {API_CONFIG.VERSION}</p>
      <StatusIndicator
        isOnline={isApiOnline}
        text={isApiOnline ? "API Connected" : "API Offline"}
      />
    </header>
  );
};
