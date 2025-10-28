import { useState, useEffect, useCallback } from "react";
import { apiService } from "@/services/api.service";
import { config } from "@/config/constants";

export const useApiHealth = () => {
  const [isOnline, setIsOnline] = useState(false);
  const [isChecking, setIsChecking] = useState(true);

  const checkHealth = useCallback(async () => {
    try {
      await apiService.checkHealth();
      setIsOnline(true);
    } catch (error) {
      setIsOnline(false);
    } finally {
      setIsChecking(false);
    }
  }, []);

  useEffect(() => {
    checkHealth();

    const interval = setInterval(checkHealth, config.apiHealthCheckInterval);

    return () => clearInterval(interval);
  }, [checkHealth]);

  return { isOnline, isChecking };
};
