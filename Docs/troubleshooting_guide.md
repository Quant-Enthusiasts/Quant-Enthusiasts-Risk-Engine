# Troubleshooting Guide

Solutions to common issues when building, deploying, or using the Quant Enthusiasts Risk Engine.

## Table of Contents

- [Build Issues](#build-issues)
- [Runtime Issues](#runtime-issues)
- [API Issues](#api-issues)
- [Market Data Issues](#market-data-issues)
- [Performance Issues](#performance-issues)
- [Platform-Specific Issues](#platform-specific-issues)

## Build Issues

### CMake Cannot Find Compiler

**Symptoms**:
```
CMake Error: CMAKE_CXX_COMPILER not set
```

**Solutions**:

1. Install a C++ compiler:
```bash
# Linux (Ubuntu/Debian)
sudo apt install build-essential

# macOS
xcode-select --install

# Check installation
g++ --version  # or clang++ --version
```

2. Specify compiler explicitly:
```bash
cmake -DCMAKE_CXX_COMPILER=g++ ..
# or
cmake -DCMAKE_CXX_COMPILER=clang++ ..
```

### CMake Version Too Old

**Symptoms**:
```
CMake 3.25 or higher is required. You are running version 3.16.3
```

**Solutions**:

**Linux**:
```bash
# Remove old CMake
sudo apt remove cmake

# Install from Kitware repository
wget -O - https://apt.kitware.com/keys/kitware-archive-latest.asc | sudo apt-key add -
sudo apt-add-repository 'deb https://apt.kitware.com/ubuntu/ focal main'
sudo apt update
sudo apt install cmake
```

**macOS**:
```bash
brew upgrade cmake
```

### pybind11 Not Found

**Symptoms**:
```
Could not find a package configuration file provided by "pybind11"
```

**Solutions**:

1. Install in virtual environment:
```bash
cd python_api
source venv/bin/activate
pip install pybind11
```

2. Install system-wide (not recommended):
```bash
pip install pybind11
```

3. Verify installation:
```bash
python -c "import pybind11; print(pybind11.get_include())"
```

### C++17 Not Supported

**Symptoms**:
```
error: invalid value 'c++17' in '-std=c++17'
```

**Solutions**:

1. Update compiler:
```bash
# Check current version
g++ --version

# Ubuntu: Install newer GCC
sudo apt install gcc-11 g++-11
sudo update-alternatives --install /usr/bin/gcc gcc /usr/bin/gcc-11 100
sudo update-alternatives --install /usr/bin/g++ g++ /usr/bin/g++-11 100
```

2. Use Clang instead:
```bash
cmake -DCMAKE_CXX_COMPILER=clang++ ..
```

### Math Constants Not Defined

**Symptoms** (Linux/macOS):
```
error: 'M_PI' was not declared in this scope
```

**Solution**:

Comment out Windows-only header in source files:
```cpp
// #include <corecrt_math_defines.h>  // Windows only
```

Or define constants manually:
```cpp
#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif
```

### Build Script Permission Denied

**Symptoms**:
```
bash: ./build.sh: Permission denied
```

**Solutions**:

1. Make executable:
```bash
chmod +x cpp_engine/build.sh
./build.sh
```

2. Run with bash directly:
```bash
bash cpp_engine/build.sh
```

### CRLF Line Ending Errors

**Symptoms** (Linux/macOS):
```
bad interpreter: /bin/bash^M: no such file or directory
```

**Solution**:

Convert line endings:
```bash
# Using perl
perl -pi -e 's/\r$//' cpp_engine/build.sh

# Using dos2unix (if available)
dos2unix cpp_engine/build.sh

# Using sed
sed -i 's/\r$//' cpp_engine/build.sh
```

## Runtime Issues

### Module Not Found Error

**Symptoms**:
```
ModuleNotFoundError: No module named 'quant_risk_engine'
```

**Solutions**:

1. Set PYTHONPATH:
```bash
export PYTHONPATH="$PWD/cpp_engine/install/lib:$PYTHONPATH"
```

2. Verify module exists:
```bash
ls -la cpp_engine/install/lib/
# Should show: quant_risk_engine.cpython-*.so
```

3. Rebuild Python bindings:
```bash
cd python_api
python setup.py build_ext --inplace
```

4. Check Python version matches build:
```bash
python --version  # Should match build Python
```

### Import Error: Symbol Not Found

**Symptoms** (macOS):
```
ImportError: dlopen(...): Symbol not found: __ZN...
```

**Solutions**:

1. Rebuild with matching Python:
```bash
cd cpp_engine/build
rm -rf *
cmake .. -DPython_EXECUTABLE=$(which python3)
cmake --build .
cmake --install .
```

2. Check library dependencies:
```bash
otool -L cpp_engine/install/lib/quant_risk_engine*.so
```

### Port Already in Use

**Symptoms**:
```
OSError: [Errno 48] Address already in use
```

**Solutions**:

1. Use different port:
```bash
python -m flask --app app run --port 5050
```

2. Find and kill process (macOS/Linux):
```bash
# Find process using port 5000
lsof -ti:5000

# Kill process
kill $(lsof -ti:5000)
```

3. Disable AirPlay Receiver (macOS):
   - System Preferences → General → AirDrop & Handoff
   - Uncheck "AirPlay Receiver"

### Segmentation Fault

**Symptoms**:
```
Segmentation fault (core dumped)
```

**Solutions**:

1. Enable debugging:
```bash
cd cpp_engine/build
cmake .. -DCMAKE_BUILD_TYPE=Debug
cmake --build .

# Run with debugger
gdb ./bin/risk-engine
(gdb) run
(gdb) bt  # backtrace when crash occurs
```

2. Check for null pointers:
```cpp
if (market_data == nullptr) {
    throw std::runtime_error("Market data is null");
}
```

3. Verify memory bounds:
```bash
# Run with valgrind
valgrind --leak-check=full ./bin/risk-engine
```

## API Issues

### Connection Refused

**Symptoms**:
```
ConnectionError: ('Connection aborted.', RemoteDisconnected('Remote end closed connection'))
```

**Solutions**:

1. Verify server is running:
```bash
curl http://127.0.0.1:5000/health
```

2. Check Flask output:
```bash
# Should show:
* Running on http://127.0.0.1:5000
```

3. Firewall blocking:
```bash
# Linux: Check firewall
sudo ufw status

# macOS: Check firewall
sudo /usr/libexec/ApplicationFirewall/socketfilterfw --getglobalstate
```

### 400 Bad Request

**Symptoms**:
```json
{
  "error": "Validation error: Portfolio item 0: missing required field 'strike'"
}
```

**Solutions**:

1. Verify JSON structure:
```bash
# Use jq to validate JSON
echo '{"portfolio": [...]}' | jq .
```

2. Check required fields:
```json
{
  "type": "call",        // Required
  "strike": 100.0,       // Required
  "expiry": 1.0,         // Required
  "asset_id": "AAPL",    // Required
  "quantity": 100,       // Required
  "style": "european"    // Required
}
```

3. Validate data types:
```python
# strike must be float, not string
"strike": 100.0  # Correct
"strike": "100"  # Wrong
```

### 500 Internal Server Error

**Symptoms**:
```json
{
  "error": "Internal server error: Risk calculation produced invalid results"
}
```

**Solutions**:

1. Check Flask logs:
```bash
# Run in debug mode
python -m flask --app app run --debug
```

2. Enable detailed error messages:
```python
# In app.py (development only)
app.config['DEBUG'] = True
app.config['PROPAGATE_EXCEPTIONS'] = True
```

3. Validate input data:
```python
# Check for NaN or Inf
import numpy as np
assert not np.isnan(spot_price)
assert not np.isinf(volatility)
```

### CORS Errors

**Symptoms** (Browser console):
```
Access to fetch at 'http://localhost:5000/calculate_risk' has been blocked by CORS policy
```

**Solutions**:

1. Verify flask-cors installed:
```bash
pip list | grep flask-cors
# If not: pip install flask-cors
```

2. Check CORS configuration:
```python
from flask_cors import CORS
CORS(app)  # Enable for all routes
```

3. Use local server for dashboard:
```bash
cd js_dashboard
npx serve .
# Access via http://localhost:3000 (not file://)
```

## Market Data Issues

### Failed to Fetch Data

**Symptoms**:
```json
{
  "failed": [
    {
      "ticker": "AAPL",
      "error": "Failed to fetch data for AAPL: No price data available"
    }
  ]
}
```

**Solutions**:

1. Verify ticker symbol:
```bash
# Check on Yahoo Finance
curl "https://finance.yahoo.com/quote/AAPL"
```

2. Check internet connectivity:
```bash
ping query1.finance.yahoo.com
```

3. Try force refresh:
```bash
curl -X POST http://localhost:5000/update_market_data \
  -d '{"tickers": ["AAPL"], "force_refresh": true}'
```

4. Check rate limiting:
```python
# Wait between requests
import time
time.sleep(1)
```

### Cache Not Updating

**Symptoms**:
Old data returned despite force refresh.

**Solutions**:

1. Clear cache:
```bash
curl -X DELETE http://localhost:5000/clear_market_data_cache
```

2. Check cache database:
```bash
sqlite3 python_api/market_data_cache.db "SELECT * FROM market_data;"
```

3. Verify write permissions:
```bash
ls -la python_api/market_data_cache.db
chmod 644 python_api/market_data_cache.db
```

4. Delete and recreate:
```bash
rm python_api/market_data_cache.db
# Cache will be recreated on next fetch
```

### Volatility Seems Incorrect

**Symptoms**:
Returned volatility differs significantly from expected.

**Solutions**:

1. Check historical data availability:
```python
import yfinance as yf
stock = yf.Ticker("AAPL")
hist = stock.history(period="1y")
print(f"Data points: {len(hist)}")
```

2. Manually specify volatility:
```json
{
  "market_data": {
    "AAPL": {
      "spot": 175.0,
      "vol": 0.28,  // Specify manually
      "rate": 0.045
    }
  }
}
```

3. Use implied volatility from options:
```python
# Calculate from option prices
implied_vol = calculate_implied_volatility(option_price, ...)
```

## Performance Issues

### Slow Risk Calculation

**Symptoms**:
Risk calculation takes > 10 seconds.

**Solutions**:

1. Reduce VaR simulations:
```json
{
  "var_parameters": {
    "simulations": 10000  // Instead of 100000
  }
}
```

2. Use Release build:
```bash
cd cpp_engine/build
cmake .. -DCMAKE_BUILD_TYPE=Release
cmake --build .
```

3. Profile code:
```bash
# C++
valgrind --tool=callgrind ./bin/risk-engine
kcachegrind callgrind.out.*

# Python
python -m cProfile app.py
```

### High Memory Usage

**Symptoms**:
Process uses > 1GB memory.

**Solutions**:

1. Reduce portfolio size:
```python
# Process in batches
for batch in chunks(portfolio, 100):
    result = calculate_risk(batch)
```

2. Limit VaR paths:
```json
{
  "var_parameters": {
    "simulations": 50000  // Reduce from default
  }
}
```

3. Check for memory leaks:
```bash
valgrind --leak-check=full --show-leak-kinds=all ./bin/risk-engine
```

### API Timeout

**Symptoms**:
```
ReadTimeout: HTTPSConnectionPool(host='query1.finance.yahoo.com'): Read timed out
```

**Solutions**:

1. Increase timeout:
```python
import requests
response = requests.post(url, json=data, timeout=60)  # 60 seconds
```

2. Use cached data:
```json
{
  "market_data": {}  // Will use cache
}
```

3. Fetch data beforehand:
```bash
# Pre-populate cache
curl -X POST http://localhost:5000/update_market_data \
  -d '{"tickers": ["AAPL", "GOOGL", ...]}'
```

## Platform-Specific Issues

### macOS: stdlib Not Found

**Symptoms**:
```
fatal error: 'stdlib.h' file not found
```

**Solutions**:

1. Install Xcode Command Line Tools:
```bash
xcode-select --install
```

2. Accept Xcode license:
```bash
sudo xcodebuild -license accept
```

3. Reset Xcode paths:
```bash
sudo xcode-select --reset
```

### Linux: Missing Math Library

**Symptoms**:
```
undefined reference to 'sqrt'
```

**Solutions**:

1. Link math library:
```cmake
# CMakeLists.txt
target_link_libraries(risk-engine m)
```

2. Compile with -lm flag:
```bash
g++ -o risk-engine main.cpp -lm
```

### Windows: DLL Not Found

**Symptoms**:
```
ImportError: DLL load failed: The specified module could not be found
```

**Solutions**:

1. Check Python architecture matches:
```powershell
python -c "import platform; print(platform.architecture())"
# Should match build (64-bit or 32-bit)
```

2. Install Visual C++ Redistributable:
```powershell
# Download from Microsoft
https://support.microsoft.com/help/2977003/the-latest-supported-visual-c-downloads
```

3. Add DLL directory to PATH:
```powershell
$env:PATH += ";C:\path\to\dlls"
```

### Windows: Build Tools Not Found

**Symptoms**:
```
error: Microsoft Visual C++ 14.0 is required
```

**Solutions**:

1. Install Visual Studio Build Tools:
```powershell
# Download from Microsoft
https://visualstudio.microsoft.com/downloads/
# Select "Build Tools for Visual Studio"
```

2. Install with required components:
   - C++ build tools
   - Windows 10 SDK
   - CMake tools

## Getting Additional Help

### Diagnostic Information

When reporting issues, include:

```bash
# System information
uname -a                    # OS version
python --version            # Python version
cmake --version             # CMake version
g++ --version              # Compiler version

# Build information
ls -la cpp_engine/install/lib/
pip list | grep -E "(flask|pybind11|yfinance)"

# Error logs
tail -n 50 python_api/logs/error.log
```

### Where to Get Help

1. **Check documentation**:
   - [Installation Guide](INSTALLATION.md)
   - [API Reference](API.md)
   - [Development Guide](DEVELOPMENT.md)

2. **Search existing issues**:
   - [GitHub Issues](https://github.com/Quant-Enthusiasts/Quant-Enthusiasts-Risk-Engine/issues)

3. **Ask community**:
   - [Discord](https://discord.com/invite/z3S9Fguzw3)
   - GitHub Discussions

4. **Report bug**:
   - Create new issue with:
     - Clear description
     - Steps to reproduce
     - System information
     - Error messages/logs

## See Also

- [Installation Guide](INSTALLATION.md) - Setup instructions
- [Quickstart](QUICKSTART.md) - Get running quickly
- [FAQ](FAQ.md) - Frequently asked questions
- [Contributing](CONTRIBUTING.md) - How to contribute