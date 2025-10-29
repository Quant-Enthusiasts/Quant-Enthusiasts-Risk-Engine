import React from "react";

export const QuickStartSection: React.FC = () => (
  <div className="space-y-6">
    <h2 className="text-3xl font-bold text-cyan-400 mb-6">Quick Start</h2>

    <div className="mb-8">
      <h3 className="text-xl font-bold text-gray-100 mb-4">Prerequisites</h3>
      <div className="code-block mb-4" style={{ whiteSpace: "pre" }}>
        {`# Check required tools
python3 --version # Need 3.11+
cmake --version # Need 3.25+
g++ --version # Need 7+ (or clang 5+)`}
      </div>
    </div>

    <div className="mb-8">
      <h3 className="text-xl font-bold text-gray-100 mb-4">
        Installation Steps
      </h3>

      <h4 className="text-lg font-semibold text-cyan-400 mb-3 mt-6">
        1. Clone Repository
      </h4>
      <div className="code-block" style={{ whiteSpace: "pre" }}>
        {`git clone https://github.com/Quant-Enthusiasts/Quant-Enthusiasts-Risk-Engine.git
cd Quant-Enthusiasts-Risk-Engine`}
      </div>

      <h4 className="text-lg font-semibold text-cyan-400 mb-3 mt-6">
        2. Build C++ Engine
      </h4>
      <div className="code-block" style={{ whiteSpace: "pre" }}>
        {`cd cpp_engine
mkdir build && cd build
cmake .. -GNinja -DCMAKE_BUILD_TYPE=Release
cmake --build .
cmake --install .
cd ../..`}
      </div>

      <h4 className="text-lg font-semibold text-cyan-400 mb-3 mt-6">
        3. Setup Python API
      </h4>
      <div className="code-block" style={{ whiteSpace: "pre" }}>
        {`cd python_api
python3 -m venv venv
source venv/bin/activate # Windows: venv\\Scripts\\activate
pip install -r requirements.txt
python setup.py build_ext --inplace`}
      </div>

      <h4 className="text-lg font-semibold text-cyan-400 mb-3 mt-6">
        4. Start Server
      </h4>
      <div className="code-block" style={{ whiteSpace: "pre" }}>
        {`export PYTHONPATH="../cpp_engine/install/lib:$PYTHONPATH"
python app.py`}
      </div>
    </div>

    <div className="bg-green-900/20 border border-green-500/50 rounded-lg p-4 mb-6">
      <p className="text-green-300 font-semibold mb-2">Server Running</p>
      <p className="text-sm text-green-200">
        API is now available at http://127.0.0.1:5000
      </p>
    </div>

    <div className="mb-8">
      <h3 className="text-xl font-bold text-gray-100 mb-4">
        Test Installation
      </h3>
      <div className="code-block" style={{ whiteSpace: "pre" }}>
        {`# Health check
curl http://127.0.0.1:5000/health

# Price an option
curl -X POST http://127.0.0.1:5000/price_option \\
  -H "Content-Type: application/json" \\
  -d '{
    "type": "call",
    "strike": 100,
    "expiry": 1.0,
    "asset_id": "AAPL",
    "style": "european",
    "market_data": {"spot": 105, "rate": 0.05, "vol": 0.25}
  }'`}
      </div>
    </div>
  </div>
);


