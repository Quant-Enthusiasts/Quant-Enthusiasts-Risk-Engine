// Environment configuration
export const config = {
  apiBaseUrl: import.meta.env.API_BASE_URL || "http://127.0.0.1:5000",
  apiHealthCheckInterval:
    Number(import.meta.env.API_HEALTH_CHECK_INTERVAL) || 30000,
  defaultMarketData: {
    spot: 100,
    rate: 0.05,
    vol: 0.25,
    dividend: 0.0,
  },
};

// API configuration
export const API_CONFIG = {
  VERSION: "3.0.0",
  NAME: "Quant Enthusiasts",
};
