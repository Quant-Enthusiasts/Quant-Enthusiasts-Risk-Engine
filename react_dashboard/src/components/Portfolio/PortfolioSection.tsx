import React from "react";
import type { Instrument } from "@/types";
import { InstrumentForm } from "./InstrumentForm";
import { PortfolioTable } from "./PortfolioTable";

interface PortfolioSectionProps {
  portfolio: Instrument[];
  onAddInstrument: (instrument: Instrument) => void;
  onRemoveInstrument: (index: number) => void;
  onError: (message: string) => void;
}

export const PortfolioSection: React.FC<PortfolioSectionProps> = ({
  portfolio,
  onAddInstrument,
  onRemoveInstrument,
  onError,
}) => {
  return (
    <section className="glass-effect rounded-xl p-6 fade-in h-full">
      <h2 className="text-lg font-bold mb-6 flex items-center gap-2">
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
              d="M21 13.255A23.931 23.931 0 0112 15c-3.183 0-6.22-.62-9-1.745M16 6V4a2 2 0 00-2-2h-4a2 2 0 00-2 2v2m4 6h.01M5 20h14a2 2 0 002-2V8a2 2 0 00-2-2H5a2 2 0 00-2 2v10a2 2 0 002 2z"
            />
          </svg>
        </span>
        <span className="text-white">Portfolio Builder</span>
      </h2>

      <InstrumentForm onAddInstrument={onAddInstrument} onError={onError} />

      <div className="mt-8">
        <h3 className="text-base font-semibold mb-4 text-gray-300">
          Current Portfolio
        </h3>
        <PortfolioTable
          portfolio={portfolio}
          onRemoveInstrument={onRemoveInstrument}
        />
      </div>
    </section>
  );
};
