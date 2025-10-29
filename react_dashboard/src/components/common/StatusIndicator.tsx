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
      <span className="text-gray-400 ml-2">{text}</span>
    </div>
  );
};
