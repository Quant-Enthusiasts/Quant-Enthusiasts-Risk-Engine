import React from "react";

interface ErrorMessageProps {
  message: string;
  onClose?: () => void;
}

export const ErrorMessage: React.FC<ErrorMessageProps> = ({
  message,
  onClose,
}) => {
  return (
    <div className="p-4 bg-red-900/30 border border-red-500/50 text-red-300 rounded-lg flex justify-between items-start">
      <p>{message}</p>
      {onClose && (
        <button
          onClick={onClose}
          className="ml-4 text-red-400 hover:text-red-300 font-bold"
          aria-label="Close error message"
        >
          ×
        </button>
      )}
    </div>
  );
};
