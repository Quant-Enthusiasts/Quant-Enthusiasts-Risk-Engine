import type { Instrument } from "@/types";

export const validateInstrument = (
  instrument: Partial<Instrument>
): string[] => {
  const errors: string[] = [];

  if (!instrument.asset_id || instrument.asset_id.trim() === "") {
    errors.push("Asset ID is required");
  }

  if (instrument.strike !== undefined && instrument.strike <= 0) {
    errors.push("Strike price must be positive");
  }

  if (instrument.expiry !== undefined && instrument.expiry <= 0) {
    errors.push("Expiry must be positive");
  }

  if (instrument.quantity !== undefined && instrument.quantity === 0) {
    errors.push("Quantity cannot be zero");
  }

  return errors;
};

export const formatValue = (
  value: number | null | undefined,
  decimals = 2
): string => {
  if (value === null || value === undefined || isNaN(value)) return "N/A";

  return value.toLocaleString("en-US", {
    minimumFractionDigits: decimals,
    maximumFractionDigits: decimals,
  });
};

export const formatNumber = formatValue;

export const getMetricColor = (label: string, value: number): string => {
  if (label.includes("VaR")) return "text-red-400";
  if (label === "Total PV")
    return value >= 0 ? "text-green-400" : "text-red-400";
  if (label === "Delta")
    return Math.abs(value) > 10 ? "text-yellow-400" : "text-cyan-400";
  return "text-cyan-400";
};

export const getRiskColor = (value: number): string => {
  if (value > 0) return "text-green-400";
  if (value < 0) return "text-red-400";
  return "text-gray-400";
};

export const getUniqueAssets = (portfolio: Instrument[]): Set<string> => {
  return new Set(portfolio.map((p) => p.asset_id));
};
