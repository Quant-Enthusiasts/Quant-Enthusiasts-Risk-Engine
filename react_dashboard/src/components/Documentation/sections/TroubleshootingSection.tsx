import React from "react";

export const TroubleshootingSection: React.FC = () => (
  <div className="space-y-6">
    <h2 className="text-3xl font-bold text-cyan-400 mb-6">Troubleshooting</h2>

    <div className="mb-8">
      <h3 className="text-xl font-bold text-gray-100 mb-4">Build Issues</h3>

      <div className="space-y-4">
        <div className="bg-gray-800/40 p-5 rounded-lg border border-gray-700/60">
          <h4 className="text-red-400 font-bold mb-2">
            CMake Cannot Find Compiler
          </h4>
          <p className="text-sm text-gray-400 mb-2">
            Error: CMAKE_CXX_COMPILER not set
          </p>
          <p className="text-sm font-semibold text-gray-300 mb-2">Solution:</p>
          <div className="code-block text-xs" style={{ whiteSpace: "pre" }}>
            {`# Install compiler
sudo apt install build-essential # Linux
xcode-select --install # macOS

# Or specify explicitly
cmake -DCMAKE_CXX_COMPILER=g++ ..`}
          </div>
        </div>

        <div className="bg-gray-800/40 p-5 rounded-lg border border-gray-700/60">
          <h4 className="text-red-400 font-bold mb-2">pybind11 Not Found</h4>
          <p className="text-sm text-gray-400 mb-2">
            Error: Could not find package configuration file
          </p>
          <p className="text-sm font-semibold text-gray-300 mb-2">Solution:</p>
          <div className="code-block text-xs" style={{ whiteSpace: "pre" }}>
            {`# Install in virtual environment
cd python_api
source venv/bin/activate
pip install pybind11`}
          </div>
        </div>

        <div className="bg-gray-800/40 p-5 rounded-lg border border-gray-700/60">
          <h4 className="text-red-400 font-bold mb-2">C++17 Not Supported</h4>
          <p className="text-sm text-gray-400 mb-2">
            Error: invalid value 'c++17' in '-std=c++17'
          </p>
          <p className="text-sm font-semibold text-gray-300 mb-2">Solution:</p>
          <div className="code-block text-xs" style={{ whiteSpace: "pre" }}>
            {`# Update compiler
sudo apt install gcc-11 g++-11

# Or use Clang
cmake -DCMAKE_CXX_COMPILER=clang++ ..`}
          </div>
        </div>
      </div>
    </div>

    <div className="mb-8">
      <h3 className="text-xl font-bold text-gray-100 mb-4">Runtime Issues</h3>

      <div className="space-y-4">
        <div className="bg-gray-800/40 p-5 rounded-lg border border-gray-700/60">
          <h4 className="text-red-400 font-bold mb-2">
            Module Not Found Error
          </h4>
          <p className="text-sm text-gray-400 mb-2">
            Error: ModuleNotFoundError: No module named 'quant_risk_engine'
          </p>
          <p className="text-sm font-semibold text-gray-300 mb-2">Solution:</p>
          <div className="code-block text-xs" style={{ whiteSpace: "pre" }}>
            {`# Set PYTHONPATH
export PYTHONPATH="$PWD/cpp_engine/install/lib:$PYTHONPATH"

# Verify module exists
ls -la cpp_engine/install/lib/quant_risk_engine*.so`}
          </div>
        </div>

        <div className="bg-gray-800/40 p-5 rounded-lg border border-gray-700/60">
          <h4 className="text-red-400 font-bold mb-2">Port Already in Use</h4>
          <p className="text-sm text-gray-400 mb-2">
            Error: Address already in use (port 5000)
          </p>
          <p className="text-sm font-semibold text-gray-300 mb-2">Solution:</p>
          <div className="code-block text-xs" style={{ whiteSpace: "pre" }}>
            {`# Use different port
python -m flask --app app run --port 5050

# Or kill process (macOS/Linux)
kill $(lsof -ti:5000)

# Disable AirPlay Receiver (macOS)
# System Preferences → General → AirPlay Receiver`}
          </div>
        </div>

        <div className="bg-gray-800/40 p-5 rounded-lg border border-gray-700/60">
          <h4 className="text-red-400 font-bold mb-2">Segmentation Fault</h4>
          <p className="text-sm text-gray-400 mb-2">
            Error: Segmentation fault (core dumped)
          </p>
          <p className="text-sm font-semibold text-gray-300 mb-2">Solution:</p>
          <div className="code-block text-xs" style={{ whiteSpace: "pre" }}>
            {`# Enable debug mode
cd cpp_engine/build
cmake .. -DCMAKE_BUILD_TYPE=Debug
cmake --build .

# Run with debugger
gdb ./bin/risk-engine
(gdb) run
(gdb) bt # backtrace on crash`}
          </div>
        </div>
      </div>
    </div>

    <div className="mb-8">
      <h3 className="text-xl font-bold text-gray-100 mb-4">API Issues</h3>

      <div className="space-y-4">
        <div className="bg-gray-800/40 p-5 rounded-lg border border-gray-700/60">
          <h4 className="text-red-400 font-bold mb-2">400 Bad Request</h4>
          <p className="text-sm text-gray-400 mb-2">
            Error: Validation error - missing required field
          </p>
          <p className="text-sm font-semibold text-gray-300 mb-2">Solution:</p>
          <ul className="text-xs text-gray-400 space-y-1 mt-2">
            <li>• Verify all required fields present</li>
            <li>• Check data types (float vs string)</li>
            <li>• Validate JSON syntax with jq</li>
            <li>• Ensure positive values where required</li>
          </ul>
        </div>

        <div className="bg-gray-800/40 p-5 rounded-lg border border-gray-700/60">
          <h4 className="text-red-400 font-bold mb-2">
            500 Internal Server Error
          </h4>
          <p className="text-sm text-gray-400 mb-2">
            Error: Risk calculation produced invalid results
          </p>
          <p className="text-sm font-semibold text-gray-300 mb-2">Solution:</p>
          <div className="code-block text-xs" style={{ whiteSpace: "pre" }}>
            {`# Run Flask in debug mode
python -m flask --app app run --debug

# Check for NaN/Inf values
# Verify market data quality
# Test with minimal portfolio first`}
          </div>
        </div>

        <div className="bg-gray-800/40 p-5 rounded-lg border border-gray-700/60">
          <h4 className="text-red-400 font-bold mb-2">CORS Errors</h4>
          <p className="text-sm text-gray-400 mb-2">
            Error: Blocked by CORS policy
          </p>
          <p className="text-sm font-semibold text-gray-300 mb-2">Solution:</p>
          <div className="code-block text-xs" style={{ whiteSpace: "pre" }}>
            {`# Verify flask-cors installed
pip list | grep flask-cors

# Serve dashboard via HTTP (not file://)
cd js_dashboard
npx serve .`}
          </div>
        </div>
      </div>
    </div>

    <div className="mb-8">
      <h3 className="text-xl font-bold text-gray-100 mb-4">
        Market Data Issues
      </h3>

      <div className="space-y-4">
        <div className="bg-gray-800/40 p-5 rounded-lg border border-gray-700/60">
          <h4 className="text-red-400 font-bold mb-2">Failed to Fetch Data</h4>
          <p className="text-sm text-gray-400 mb-2">
            Error: No price data available
          </p>
          <p className="text-sm font-semibold text-gray-300 mb-2">Solutions:</p>
          <ul className="text-xs text-gray-400 space-y-1 mt-2">
            <li>• Verify ticker on finance.yahoo.com</li>
            <li>• Check internet connectivity</li>
            <li>• Try force_refresh: true</li>
            <li>• Provide data manually as fallback</li>
          </ul>
        </div>

        <div className="bg-gray-800/40 p-5 rounded-lg border border-gray-700/60">
          <h4 className="text-red-400 font-bold mb-2">
            Volatility Seems Incorrect
          </h4>
          <p className="text-sm text-gray-400 mb-2">
            Returned volatility differs from expected
          </p>
          <p className="text-sm font-semibold text-gray-300 mb-2">Solution:</p>
          <div className="code-block text-xs" style={{ whiteSpace: "pre" }}>
            {`# Manually specify volatility
{
  "market_data": {
    "AAPL": {
      "spot": 175.0,
      "vol": 0.28, // Override
      "rate": 0.045
    }
  }
}`}
          </div>
        </div>
      </div>
    </div>
  </div>
);
