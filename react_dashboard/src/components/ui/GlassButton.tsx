import React from "react";

interface GlassButtonProps {
  onClick: () => void;
  icon: React.ReactNode;
  label: string;
  mobileLabel?: string;
  title?: string;
}

export const GlassButton: React.FC<GlassButtonProps> = ({
  onClick,
  icon,
  label,
  mobileLabel,
  title,
}) => {
  return (
    <>
      {/* Desktop Navigation (Top Right) */}
      <div className="fixed top-6 right-6 z-50 hidden md:block">
        <button
          onClick={onClick}
          className="glass-nav-button px-6 py-3 rounded-xl font-semibold text-sm flex items-center gap-3 hover:scale-105 transform transition-all duration-300 shadow-xl hover:shadow-cyan-500/50"
          title={title}
        >
          {icon}
          <span>{label}</span>
        </button>
      </div>

      {/* Mobile Navigation (Bottom) */}
      <div className="fixed bottom-6 left-1/2 transform -translate-x-1/2 z-50 md:hidden">
        <button
          onClick={onClick}
          className="glass-nav-button px-8 py-4 rounded-full font-semibold text-sm flex items-center gap-3 hover:scale-105 transform transition-all duration-300 shadow-2xl hover:shadow-cyan-500/50"
          title={title}
        >
          {icon}
          <span>{mobileLabel || label}</span>
        </button>
      </div>

      <style>{`
        .glass-nav-button {
          background: rgba(15, 23, 42, 0.95);
          backdrop-filter: blur(20px) saturate(180%);
          -webkit-backdrop-filter: blur(20px) saturate(180%);
          border: 1px solid rgba(6, 182, 212, 0.3);
          color: #06b6d4;
          cursor: pointer;
          position: relative;
          overflow: hidden;
        }

        .glass-nav-button::before {
          content: '';
          position: absolute;
          top: 0;
          left: -100%;
          width: 100%;
          height: 100%;
          background: linear-gradient(90deg, transparent, rgba(6, 182, 212, 0.3), transparent);
          transition: left 0.5s ease;
        }

        .glass-nav-button:hover::before {
          left: 100%;
        }

        .glass-nav-button:hover {
          border-color: rgba(6, 182, 212, 0.6);
          color: #22d3ee;
        }

        .glass-nav-button:active {
          transform: scale(0.95) !important;
        }

        @media (max-width: 768px) {
          .glass-nav-button {
            animation: slideUpFade 0.5s ease-out;
          }

          body {
            padding-bottom: 100px;
          }
        }

        @keyframes slideUpFade {
          from {
            opacity: 0;
            transform: translateY(20px);
          }
          to {
            opacity: 1;
            transform: translateY(0);
          }
        }
      `}</style>
    </>
  );
};
