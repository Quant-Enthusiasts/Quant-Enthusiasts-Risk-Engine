export type Section =
  | "overview"
  | "quickstart"
  | "architecture"
  | "api"
  | "pricing"
  | "risk"
  | "market-data"
  | "deployment"
  | "troubleshooting";

export interface SectionMetadata {
  id: Section;
  name: string;
  icon: string;
}

export const sections: SectionMetadata[] = [
  { id: "overview", name: "Overview", icon: "🏠" },
  { id: "quickstart", name: "Quick Start", icon: "🚀" },
  { id: "architecture", name: "Architecture", icon: "🏗️" },
  { id: "api", name: "API Reference", icon: "📡" },
  { id: "pricing", name: "Pricing Models", icon: "💰" },
  { id: "risk", name: "Risk Metrics", icon: "📊" },
  { id: "market-data", name: "Market Data", icon: "📈" },
  { id: "deployment", name: "Deployment", icon: "🚀" },
  { id: "troubleshooting", name: "Troubleshooting", icon: "🔧" },
];
