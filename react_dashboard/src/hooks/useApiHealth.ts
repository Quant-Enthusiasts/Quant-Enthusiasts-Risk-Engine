import { useState, useEffect, useCallback } from "react";
import { apiService } from "@/services/api.service";
import { config } from "@/config/constants";
import type { ApiHealthResponse } from "@/types";

export const useApiHealth = () => {
  const [isOnline, setIsOnline] = useState(false);
  const [isChecking, setIsChecking] = useState(true);
  const [healthData, setHealthData] = useState<ApiHealthResponse | null>(null);

  const checkHealth = useCallback(async () => {
    try {
      const data = await apiService.checkHealth();
      setHealthData(data);
      setIsOnline(true);
    } catch (error) {
      setIsOnline(false);
      setHealthData(null);
    } finally {
      setIsChecking(false);
    }
  }, []);

  useEffect(() => {
    checkHealth();

    const interval = setInterval(checkHealth, config.apiHealthCheckInterval);

    return () => clearInterval(interval);
  }, [checkHealth]);

  return { isOnline, isChecking, healthData };
};
