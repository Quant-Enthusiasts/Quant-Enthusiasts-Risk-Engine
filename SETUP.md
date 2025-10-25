# Quant Enthusiasts Risk Engine - Quick Setup Guide

Get the full project (C++ core + Python API + JS dashboard) running in under 15 minutes. This guide covers building, running, and testing for new contributors.

## Prerequisites (2 minutes)

- **macOS/Linux/Windows**: Any OS with a C++17 compiler.
- **Python 3.11+**: Install from python.org or via Homebrew (`brew install python@3.11`).
- **Git**: Clone the repo (`git clone https://github.com/Quant-Enthusiasts/Quant-Enthusiasts-Risk-Engine.git`).
- **CMake 3.25+**: Install via Homebrew (`brew install cmake`) or from cmake.org.
- **Optional**: Node.js for dashboard (`brew install node`).

## Quick Setup (<15 minutes)

### 1. Clone and Setup Python Environment (2 minutes)

```bash
cd Quant-Enthusiasts-Risk-Engine
python3 -m venv .venv
source .venv/bin/activate  # Linux/Mac; Windows: .venv\Scripts\activate
pip install --upgrade pip
pip install -r python_api/requirements.txt
pip install pybind11
```

### 2. Build C++ Engine (3 minutes)

Fix line endings (Windows clone issue), then build:

```bash
# Fix Windows CRLF line endings in build script
perl -pi -e 's/\r$//' cpp_engine/build.sh

# Build everything (clean, compile, test)
cd cpp_engine
./build.sh --all
cd ..
```

This creates:
- `cpp_engine/build/bin/risk-engine` (C++ demo executable)
- `cpp_engine/build/quant_risk_engine.cpython-*.so` (Python module)

### 3. Build Python Bindings (1 minute)

```bash
cd python_api
python setup.py build_ext --inplace
cd ..
```

### 4. Run API Server (2 minutes)

```bash
# Set PYTHONPATH so Flask finds the C++ module
export PYTHONPATH="$PWD/cpp_engine/build:$PYTHONPATH"

# Start Flask on port 5050 (avoids macOS port 5000 conflict)
python -m flask --app python_api.app run --port 5050 --debug
```

Server runs at: `http://127.0.0.1:5050`

### 5. Test Everything (5 minutes)

In a new terminal:

```bash
# Test C++ demo
./cpp_engine/build/bin/risk-engine

# Test API health
curl http://127.0.0.1:5050/health

# Test option pricing
curl -X POST http://127.0.0.1:5050/price_option \
  -H "Content-Type: application/json" \
  -d '{"type":"call","strike":100,"expiry":1.0,"asset_id":"AAPL","style":"european","pricing_model":"blackscholes","market_data":{"spot":105,"rate":0.05,"vol":0.25}}'

# Test portfolio risk
curl -X POST http://127.0.0.1:5050/calculate_risk \
  -H "Content-Type: application/json" \
  -d '{"portfolio":[{"type":"call","strike":100,"expiry":1.0,"asset_id":"AAPL","quantity":100,"style":"european"}],"market_data":{"AAPL":{"spot":105,"rate":0.05,"vol":0.25}},"var_parameters":{"simulations":10000,"confidence":0.95,"time_horizon":1.0}}'
```

### 6. Run Dashboard (Optional, 2 minutes)

```bash
cd js_dashboard
npx serve .
# Open http://localhost:3000 in browser
```

## Common Build Errors & Fixes

### "bad interpreter: /bin/bash^M"
- **Cause**: Windows line endings in `build.sh`.
- **Fix**: `perl -pi -e 's/\r$//' cpp_engine/build.sh`

### "CMake Error: pybind11 not found"
- **Cause**: pybind11 not installed in Python environment.
- **Fix**: `pip install pybind11` in activated venv.

### "ModuleNotFoundError: quant_risk_engine"
- **Cause**: PYTHONPATH not set for Flask.
- **Fix**: `export PYTHONPATH="$PWD/cpp_engine/build:$PYTHONPATH"` before running Flask.

### "Port 5000 already in use"
- **Cause**: macOS AirPlay or other service using port 5000.
- **Fix**: Use port 5050: `python -m flask --app python_api.app run --port 5050 --debug`

### Build fails with C++17 errors
- **Cause**: Old compiler (needs GCC 7+, Clang 5+, MSVC 2017+).
- **Fix**: Update compiler or specify: `cmake -DCMAKE_CXX_STANDARD=17 ..`

### "Permission denied" on build.sh
- **Cause**: Script not executable.
- **Fix**: `chmod +x cpp_engine/build.sh`

### CMake can't find Python
- **Cause**: Wrong Python version or not in PATH.
- **Fix**: Use full path: `cmake -DPython_EXECUTABLE=/path/to/python3 ..`

## Troubleshooting

- **Clean rebuild**: `cd cpp_engine && ./build.sh --clean && ./build.sh --all`
- **Check dependencies**: `python --version`, `cmake --version`, `g++ --version`
- **Logs**: Flask debug mode shows errors. C++ build outputs to console.
- **Windows**: Use PowerShell, replace `source` with `.\venv\Scripts\activate`, use `python` instead of `python3`.

## What You Built

- **C++ Core**: High-performance options pricing (Black-Scholes, Binomial, Jump Diffusion) + Monte Carlo VaR.
- **Python API**: REST endpoints for pricing/risk calc, built with Flask.
- **Tests**: Unit tests for all core functions (run via `./build.sh --all`).
- **Dashboard**: Simple web UI to build portfolios and visualize risk.

## Next Steps

- Read `README.md` for detailed API docs.
- Explore `cpp_engine/src/` for quant algorithms.
- Contribute: Fix bugs, add features, improve docs.

Setup time: <5 minutes. Questions? Open an issue!
