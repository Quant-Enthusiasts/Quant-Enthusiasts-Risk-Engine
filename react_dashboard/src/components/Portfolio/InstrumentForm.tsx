import React, { useState } from "react";
import type { Instrument, OptionStyle, OptionType } from "@/types";
import { validateInstrument } from "@/utils/helpers";

interface InstrumentFormProps {
  onAddInstrument: (instrument: Instrument) => void;
  onError: (message: string) => void;
}

export const InstrumentForm: React.FC<InstrumentFormProps> = ({
  onAddInstrument,
  onError,
}) => {
  const [formData, setFormData] = useState({
    assetId: "AAPL",
    style: "european" as OptionStyle,
    type: "call" as OptionType,
    strike: "100",
    expiry: "1.0",
    quantity: "100",
  });

  const handleSubmit = (e: React.FormEvent) => {
    e.preventDefault();

    const newInstrument: Instrument = {
      asset_id: formData.assetId.trim().toUpperCase(),
      style: formData.style,
      type: formData.type,
      strike: parseFloat(formData.strike),
      expiry: parseFloat(formData.expiry),
      quantity: parseInt(formData.quantity),
    };

    const errors = validateInstrument(newInstrument);
    if (errors.length > 0) {
      onError(`Validation error: ${errors.join(", ")}`);
      return;
    }

    onAddInstrument(newInstrument);

    // Reset form but keep asset ID
    setFormData({
      ...formData,
      strike: "100",
      expiry: "1.0",
      quantity: "100",
      style: "european",
      type: "call",
    });
  };

  const handleChange = (field: string, value: string) => {
    setFormData((prev) => ({ ...prev, [field]: value }));
  };

  return (
    <form
      onSubmit={handleSubmit}
      className="grid grid-cols-1 sm:grid-cols-2 lg:grid-cols-3 gap-5 mb-6"
    >
      <div>
        <label
          htmlFor="asset-id"
          className="block text-xs font-medium text-gray-400 mb-1"
        >
          Asset ID
        </label>
        <input
          type="text"
          id="asset-id"
          value={formData.assetId}
          onChange={(e) => handleChange("assetId", e.target.value)}
          className="w-full bg-gray-800/60 border border-gray-700 rounded-lg px-3 py-2 text-sm text-white placeholder-gray-500 focus:ring-2 focus:ring-cyan-500 focus:border-transparent"
          placeholder="e.g., AAPL"
          required
        />
      </div>

      <div>
        <label
          htmlFor="style"
          className="block text-xs font-medium text-gray-400 mb-1"
        >
          Style
        </label>
        <select
          id="style"
          value={formData.style}
          onChange={(e) => handleChange("style", e.target.value)}
          className="custom-select w-full bg-gray-800/60 border border-gray-700 rounded-lg px-3 py-2 text-sm text-white focus:ring-2 focus:ring-cyan-500 focus:border-transparent"
        >
          <option value="european">European</option>
          <option value="american">American</option>
        </select>
      </div>

      <div>
        <label
          htmlFor="type"
          className="block text-xs font-medium text-gray-400 mb-1"
        >
          Type
        </label>
        <select
          id="type"
          value={formData.type}
          onChange={(e) => handleChange("type", e.target.value)}
          className="custom-select w-full bg-gray-800/60 border border-gray-700 rounded-lg px-3 py-2 text-sm text-white focus:ring-2 focus:ring-cyan-500 focus:border-transparent"
        >
          <option value="call">Call</option>
          <option value="put">Put</option>
        </select>
      </div>

      <div>
        <label
          htmlFor="strike"
          className="block text-xs font-medium text-gray-400 mb-1"
        >
          Strike Price
        </label>
        <input
          type="number"
          id="strike"
          value={formData.strike}
          onChange={(e) => handleChange("strike", e.target.value)}
          step="0.01"
          min="0.01"
          className="w-full bg-gray-800/60 border border-gray-700 rounded-lg px-3 py-2 text-sm text-white focus:ring-2 focus:ring-cyan-500 focus:border-transparent"
          required
        />
      </div>

      <div>
        <label
          htmlFor="expiry"
          className="block text-xs font-medium text-gray-400 mb-1"
        >
          Expiry (Years)
        </label>
        <input
          type="number"
          id="expiry"
          value={formData.expiry}
          onChange={(e) => handleChange("expiry", e.target.value)}
          step="0.01"
          min="0.01"
          className="w-full bg-gray-800/60 border border-gray-700 rounded-lg px-3 py-2 text-sm text-white focus:ring-2 focus:ring-cyan-500 focus:border-transparent"
          required
        />
      </div>

      <div>
        <label
          htmlFor="quantity"
          className="block text-xs font-medium text-gray-400 mb-1"
        >
          Quantity
        </label>
        <input
          type="number"
          id="quantity"
          value={formData.quantity}
          onChange={(e) => handleChange("quantity", e.target.value)}
          className="w-full bg-gray-800/60 border border-gray-700 rounded-lg px-3 py-2 text-sm text-white focus:ring-2 focus:ring-cyan-500 focus:border-transparent"
          required
        />
      </div>

      <div className="sm:col-span-2 lg:col-span-3">
        <button
          type="submit"
          className="w-full bg-gradient-to-r from-cyan-600 to-blue-600 hover:from-cyan-500 hover:to-blue-500 text-white font-semibold py-2.5 px-6 rounded-lg shadow-lg hover:shadow-cyan-500/50 transform hover:-translate-y-0.5 active:scale-95 flex items-center justify-center gap-2"
        >
          <svg
            className="w-4 h-4"
            fill="none"
            stroke="currentColor"
            viewBox="0 0 24 24"
          >
            <path
              strokeLinecap="round"
              strokeLinejoin="round"
              strokeWidth="2"
              d="M12 4v16m8-8H4"
            />
          </svg>
          Add to Portfolio
        </button>
      </div>
    </form>
  );
};
