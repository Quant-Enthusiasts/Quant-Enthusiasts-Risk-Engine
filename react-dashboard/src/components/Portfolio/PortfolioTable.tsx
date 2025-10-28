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
          <tr className="border-b border-gray-700/50">
            <th className="px-4 py-3 text-left text-xs font-semibold text-gray-400 uppercase tracking-wider">
              Asset
            </th>
            <th className="px-4 py-3 text-left text-xs font-semibold text-gray-400 uppercase tracking-wider">
              Style
            </th>
            <th className="px-4 py-3 text-left text-xs font-semibold text-gray-400 uppercase tracking-wider">
              Type
            </th>
            <th className="px-4 py-3 text-left text-xs font-semibold text-gray-400 uppercase tracking-wider">
              Quantity
            </th>
            <th className="px-4 py-3 text-right text-xs font-semibold text-gray-400 uppercase tracking-wider">
              Strike
            </th>
            <th className="px-4 py-3 text-right text-xs font-semibold text-gray-400 uppercase tracking-wider">
              Expiry
            </th>
            <th className="px-4 py-3 text-center text-xs font-semibold text-gray-400 uppercase tracking-wider">
              Actions
            </th>
          </tr>
        </thead>
        <tbody className="bg-gray-900/30">
          {portfolio.map((instrument, index) => (
            <tr
              key={index}
              className="border-b border-gray-700/30 hover:bg-gray-800/40 transition-colors"
            >
              <td className="px-4 py-4 text-cyan-400 font-medium">
                {instrument.asset_id}
              </td>
              <td className="px-4 py-4">
                <span className="inline-block px-3 py-1 rounded-full text-xs font-medium bg-purple-500/20 text-purple-300">
                  {instrument.style}
                </span>
              </td>
              <td className="px-4 py-4">
                <span
                  className={`inline-block px-3 py-1 rounded-full text-xs font-medium ${
                    instrument.type === "call"
                      ? "bg-green-500/20 text-green-400"
                      : "bg-red-500/20 text-red-400"
                  }`}
                >
                  {instrument.type}
                </span>
              </td>
              <td className="px-4 py-4 text-green-400 font-medium">
                +{instrument.quantity}
              </td>
              <td className="px-4 py-4 text-right text-gray-300">
                ${formatNumber(instrument.strike)}
              </td>
              <td className="px-4 py-4 text-right text-gray-300">
                {formatNumber(instrument.expiry)}y
              </td>
              <td className="px-4 py-4 text-center">
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
