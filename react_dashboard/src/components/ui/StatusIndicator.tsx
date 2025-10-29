import React from "react";

interface StatusIndicatorProps {
  isOnline: boolean;
  text: string;
}

export const StatusIndicator: React.FC<StatusIndicatorProps> = ({
  isOnline,
  text,
}) => {
  return (
    <div className="flex items-center justify-center text-sm">
      <span
        className={`status-indicator ${
          isOnline ? "status-online" : "status-offline"
        }`}
      />
      <span className={`${isOnline ? "text-gray-400" : "text-gray-400"}`}>
        {text}
      </span>
    </div>
  );
};
