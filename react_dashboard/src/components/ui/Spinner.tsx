import React from "react";

interface SpinnerProps {
  message?: string;
}

export const Spinner: React.FC<SpinnerProps> = ({ message = "Loading..." }) => {
  return (
    <div className="flex flex-col justify-center items-center py-12">
      <div className="animate-spin rounded-full h-16 w-16 border-t-4 border-b-4 border-cyan-500 mb-4" />
      <p className="text-gray-400 animate-pulse">{message}</p>
    </div>
  );
};
