// Environment configuration
export const config = {
  apiBaseUrl: import.meta.env.VITE_API_BASE_URL || "http://10.81.36.164:10000",
  apiHealthCheckInterval:
    Number(import.meta.env.VITE_API_HEALTH_CHECK_INTERVAL) || 30000,
  defaultMarketData: {
    spot: 100,
    rate: 0.05,
    vol: 0.25,
    dividend: 0.0,
  },
};

// API configuration
export const API_CONFIG = {
  VERSION: "3.1.0",
  NAME: "Quant Enthusiasts",
};
