export type OptionStyle = "european" | "american";
export type OptionType = "call" | "put";
export type PricingModel = "blackscholes" | "binomial" | "jumpdiffusion";

export interface Instrument {
  asset_id: string;
  style: OptionStyle;
  type: OptionType;
  strike: number;
  expiry: number;
  quantity: number;
  pricing_model?: PricingModel;
  binomial_steps?: number;
  jump_parameters?: JumpParameters;
}

export interface JumpParameters {
  lambda: number;
  mean: number;
  vol: number;
}

export interface MarketData {
  spot: number;
  rate: number;
  vol: number;
  dividend: number;
}

export type MarketDataMap = Record<string, MarketData>;

export interface VarParameters {
  simulations: number;
  confidence: number;
  time_horizon: number;
  seed?: number;
}

export interface RiskCalculationRequest {
  portfolio: Instrument[];
  market_data: MarketDataMap;
  var_parameters?: VarParameters;
}

export interface RiskMetrics {
  total_pv: number;
  total_delta: number;
  total_gamma: number;
  total_vega: number;
  total_theta: number;
  value_at_risk_95: number;
  portfolio_size: number;
  var_parameters?: {
    simulations: number;
    confidence_level: number;
    time_horizon_days: number;
  };
}

export interface ApiHealthResponse {
  status: string;
  service: string;
  version: string;
  features: string[];
}

export interface ApiErrorResponse {
  error: string;
}
