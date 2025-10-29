import React from "react";
import { getRiskColor } from "@/utils/helpers";

interface MetricCardProps {
  label: string;
  value: number | string;
  isRisk?: boolean;
}

export const MetricCard: React.FC<MetricCardProps> = ({
  label,
  value,
  isRisk = false,
}) => {
  const numValue = typeof value === "number" ? value : parseFloat(value);
  const colorClass = isRisk ? getRiskColor(numValue) : "text-cyan-400";

  return (
    <div className="bg-gray-800/50 rounded-lg p-5 border border-gray-700 hover:border-cyan-500/50 transition-all">
      <div className="text-sm text-gray-400 mb-2">{label}</div>
      <div className={`text-2xl font-bold ${colorClass}`}>
        {typeof value === "number" ? value.toFixed(4) : value}
      </div>
    </div>
  );
};
