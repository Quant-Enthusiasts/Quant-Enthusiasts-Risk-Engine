import axios, { AxiosInstance, AxiosError } from "axios";
import { config } from "@/config/constants";
import type {
  RiskCalculationRequest,
  RiskMetrics,
  ApiHealthResponse,
  ApiErrorResponse,
} from "@/types";

class ApiService {
  private client: AxiosInstance;

  constructor() {
    this.client = axios.create({
      baseURL: config.apiBaseUrl,
      headers: {
        "Content-Type": "application/json",
      },
      timeout: 30000,
    });
  }

  /**
   * Check API health status
   */
  async checkHealth(): Promise<ApiHealthResponse> {
    try {
      const response = await this.client.get<ApiHealthResponse>("/health");
      return response.data;
    } catch (error) {
      throw this.handleError(error);
    }
  }

  /**
   * Calculate portfolio risk metrics
   */
  async calculateRisk(request: RiskCalculationRequest): Promise<RiskMetrics> {
    try {
      const response = await this.client.post<RiskMetrics>(
        "/calculate_risk",
        request
      );
      return response.data;
    } catch (error) {
      throw this.handleError(error);
    }
  }

  /**
   * Handle API errors
   */
  private handleError(error: unknown): Error {
    if (axios.isAxiosError(error)) {
      const axiosError = error as AxiosError<ApiErrorResponse>;

      if (axiosError.response?.data?.error) {
        return new Error(axiosError.response.data.error);
      }

      if (axiosError.response) {
        return new Error(
          `HTTP ${axiosError.response.status}: ${axiosError.response.statusText}`
        );
      }

      if (axiosError.request) {
        return new Error("No response from server. Ensure the API is running.");
      }
    }

    return new Error("An unexpected error occurred");
  }
}

export const apiService = new ApiService();


export const calculateRisk = async (
  portfolio: RiskCalculationRequest["portfolio"],
  marketData: RiskCalculationRequest["market_data"]
): Promise<RiskMetrics> => {
  return apiService.calculateRisk({ portfolio, market_data: marketData });
};
