import React, { useState } from "react";
import { useNavigate } from "react-router-dom";
import { Section, sections } from "../data/documentationSections";
import { GlassButton } from "../components/ui/GlassButton";
import {
  OverviewSection,
  QuickStartSection,
  ArchitectureSection,
  APISection,
  PricingSection,
  RiskSection,
  MarketDataSection,
  DeploymentSection,
  TroubleshootingSection,
} from "../components/Documentation/sections";

export const Documentation: React.FC = () => {
  const navigate = useNavigate();
  const [activeSection, setActiveSection] = useState<Section>("overview");

  const renderSection = () => {
    switch (activeSection) {
      case "overview":
        return <OverviewSection />;
      case "quickstart":
        return <QuickStartSection />;
      case "architecture":
        return <ArchitectureSection />;
      case "api":
        return <APISection />;
      case "pricing":
        return <PricingSection />;
      case "risk":
        return <RiskSection />;
      case "market-data":
        return <MarketDataSection />;
      case "deployment":
        return <DeploymentSection />;
      case "troubleshooting":
        return <TroubleshootingSection />;
      default:
        return <OverviewSection />;
    }
  };

  return (
    <div className="min-h-screen bg-gradient-to-br from-gray-900 via-slate-900 to-gray-900">
      <GlassButton
        onClick={() => navigate("/")}
        icon={
          <svg
            className="w-5 h-5"
            fill="none"
            stroke="currentColor"
            viewBox="0 0 24 24"
          >
            <path
              strokeLinecap="round"
              strokeLinejoin="round"
              strokeWidth="2"
              d="M3 12l2-2m0 0l7-7 7 7M5 10v10a1 1 0 001 1h3m10-11l2 2m-2-2v10a1 1 0 01-1 1h-3m-6 0a1 1 0 001-1v-4a1 1 0 011-1h2a1 1 0 011 1v4a1 1 0 001 1m-6 0h6"
            />
          </svg>
        }
        label="Dashboard"
        title="Back to Dashboard"
      />

      <div className="container mx-auto p-4 md:p-8 max-w-7xl">
        <header className="text-center mb-12 fade-in">
          <h1 className="text-4xl md:text-5xl font-extrabold gradient-text mb-4">
            Quant Enthusiasts Risk Engine
          </h1>
          <p className="text-lg text-gray-400">
            Complete Documentation &amp; API Reference
          </p>
        </header>

        <div className="grid grid-cols-1 lg:grid-cols-4 gap-8">
          <aside className="lg:col-span-1">
            <nav className="glass-effect rounded-xl p-6 sticky top-6">
              <h2 className="text-lg font-bold text-cyan-400 mb-4">Contents</h2>
              <ul className="space-y-2">
                {sections.map((section) => (
                  <li key={section.id}>
                    <button
                      onClick={() => setActiveSection(section.id)}
                      className={`w-full text-left px-4 py-2 rounded-lg transition-all ${
                        activeSection === section.id
                          ? "bg-cyan-500/20 text-cyan-400 border-l-4 border-cyan-400"
                          : "text-gray-400 hover:bg-gray-800/50 hover:text-cyan-300"
                      }`}
                    >
                      <span className="mr-2">{section.icon}</span>
                      {section.name}
                    </button>
                  </li>
                ))}
              </ul>
            </nav>
          </aside>
          <main className="lg:col-span-3">
            <div className="glass-effect rounded-xl p-8">{renderSection()}</div>
          </main>
        </div>
      </div>

      <style>{`
        .gradient-text {
          background: linear-gradient(135deg, #06b6d4, #3b82f6, #8b5cf6);
          -webkit-background-clip: text;
          -webkit-text-fill-color: transparent;
          background-clip: text;
        }

        .glass-effect {
          background: rgba(15, 23, 42, 0.85);
          backdrop-filter: blur(20px) saturate(180%);
          -webkit-backdrop-filter: blur(20px) saturate(180%);
          border: 1px solid rgba(255, 255, 255, 0.08);
        }

        .code-block {
          background: rgba(0, 0, 0, 0.4);
          border: 1px solid rgba(6, 182, 212, 0.2);
          border-radius: 0.5rem;
          padding: 1rem;
          font-family: 'Courier New', monospace;
          font-size: 0.875rem;
          color: #e5e7eb;
          overflow-x: auto;
        }

        .fade-in {
          animation: fadeIn 0.5s ease-in;
        }

        @keyframes fadeIn {
          from { opacity: 0; transform: translateY(10px); }
          to { opacity: 1; transform: translateY(0); }
        }
      `}</style>
    </div>
  );
};
