import React from "react";

export const DocumentationStyles: React.FC = () => (
  <style>{`
    .gradient-text {
      background: linear-gradient(135deg, #06b6d4, #3b82f6, #8b5cf6);
      -webkit-background-clip: text;
      -webkit-text-fill-color: transparent;
      background-clip: text;
    }

    .glass-effect {
      background: rgba(15, 23, 42, 0.85);
      backdrop-filter: blur(20px) saturate(180%);
      -webkit-backdrop-filter: blur(20px) saturate(180%);
      border: 1px solid rgba(255, 255, 255, 0.08);
    }

    .nav-button {
      background: rgba(15, 23, 42, 0.95);
      backdrop-filter: blur(20px) saturate(180%);
      -webkit-backdrop-filter: blur(20px) saturate(180%);
      border: 1px solid rgba(6, 182, 212, 0.3);
      color: #06b6d4;
      cursor: pointer;
      position: relative;
      overflow: hidden;
    }

    .nav-button::before {
      content: '';
      position: absolute;
      top: 0;
      left: -100%;
      width: 100%;
      height: 100%;
      background: linear-gradient(90deg, transparent, rgba(6, 182, 212, 0.3), transparent);
      transition: left 0.5s ease;
    }

    .nav-button:hover::before {
      left: 100%;
    }

    .nav-button:hover {
      border-color: rgba(6, 182, 212, 0.6);
      color: #22d3ee;
    }

    .code-block {
      background: rgba(0, 0, 0, 0.4);
      border: 1px solid rgba(6, 182, 212, 0.2);
      border-radius: 0.5rem;
      padding: 1rem;
      font-family: 'Courier New', monospace;
      font-size: 0.875rem;
      color: #e5e7eb;
      overflow-x: auto;
    }

    .fade-in {
      animation: fadeIn 0.5s ease-in;
    }

    @keyframes fadeIn {
      from { opacity: 0; transform: translateY(10px); }
      to { opacity: 1; transform: translateY(0); }
    }
  `}</style>
);
