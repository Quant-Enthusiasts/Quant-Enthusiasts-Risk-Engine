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
    <section className="glass-effect rounded-xl p-6 fade-in">
      <h2 className="text-2xl font-bold mb-6 flex items-center gap-2">
        <span className="text-2xl">💼</span>
        <span className="text-transparent bg-clip-text bg-gradient-to-r from-cyan-400 to-blue-400">
          Portfolio Builder
        </span>
      </h2>

      <InstrumentForm onAddInstrument={onAddInstrument} onError={onError} />

      <div className="mt-8">
        <h3 className="text-xl font-semibold mb-4 text-gray-200">
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
