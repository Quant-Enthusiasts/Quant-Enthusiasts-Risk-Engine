import React from "react";
import type { Instrument } from "@/types";
import { formatNumber } from "@/utils/helpers";

interface PortfolioTableProps {
  portfolio: Instrument[];
  onRemoveInstrument: (index: number) => void;
}

export const PortfolioTable: React.FC<PortfolioTableProps> = ({
  portfolio,
  onRemoveInstrument,
}) => {
  if (portfolio.length === 0) {
    return (
      <div className="text-center py-8 text-gray-400">
        <p className="text-lg">📊 No instruments in portfolio yet</p>
        <p className="text-sm mt-2">Add instruments above to get started</p>
      </div>
    );
  }

  return (
    <div className="overflow-x-auto">
      <table className="w-full">
        <thead>
          <tr className="border-b border-gray-700">
            <th className="px-4 py-3 text-left text-sm font-semibold text-gray-300">
              Asset
            </th>
            <th className="px-4 py-3 text-left text-sm font-semibold text-gray-300">
              Style
            </th>
            <th className="px-4 py-3 text-left text-sm font-semibold text-gray-300">
              Type
            </th>
            <th className="px-4 py-3 text-right text-sm font-semibold text-gray-300">
              Strike
            </th>
            <th className="px-4 py-3 text-right text-sm font-semibold text-gray-300">
              Expiry (Y)
            </th>
            <th className="px-4 py-3 text-right text-sm font-semibold text-gray-300">
              Quantity
            </th>
            <th className="px-4 py-3 text-center text-sm font-semibold text-gray-300">
              Action
            </th>
          </tr>
        </thead>
        <tbody>
          {portfolio.map((instrument, index) => (
            <tr
              key={index}
              className="border-b border-gray-700/50 hover:bg-gray-800/30 transition-colors"
            >
              <td className="px-4 py-3 text-white font-medium">
                {instrument.asset_id}
              </td>
              <td className="px-4 py-3 text-gray-300 capitalize">
                {instrument.style}
              </td>
              <td className="px-4 py-3">
                <span
                  className={`inline-block px-2 py-1 rounded text-xs font-semibold ${
                    instrument.type === "call"
                      ? "bg-green-500/20 text-green-400"
                      : "bg-red-500/20 text-red-400"
                  }`}
                >
                  {instrument.type.toUpperCase()}
                </span>
              </td>
              <td className="px-4 py-3 text-right text-gray-300">
                ${formatNumber(instrument.strike)}
              </td>
              <td className="px-4 py-3 text-right text-gray-300">
                {formatNumber(instrument.expiry)}
              </td>
              <td className="px-4 py-3 text-right text-gray-300">
                {instrument.quantity}
              </td>
              <td className="px-4 py-3 text-center">
                <button
                  onClick={() => onRemoveInstrument(index)}
                  className="text-red-400 hover:text-red-300 hover:bg-red-500/20 px-3 py-1 rounded transition-colors"
                  title="Remove instrument"
                >
                  🗑️
                </button>
              </td>
            </tr>
          ))}
        </tbody>
      </table>
      <div className="mt-4 text-sm text-gray-400">
        <span className="font-semibold text-cyan-400">{portfolio.length}</span>{" "}
        instrument
        {portfolio.length !== 1 ? "s" : ""} in portfolio
      </div>
    </div>
  );
};
