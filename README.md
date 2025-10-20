<div align="center">

# Quant Enthusiasts Risk Engine

**Modular, high-performance quantitative finance risk engine**

[![License: MIT](https://img.shields.io/badge/License-MIT-blue.svg)](https://opensource.org/licenses/MIT)
[![C++17](https://img.shields.io/badge/C++-17-00599C?logo=cplusplus)](https://isocpp.org/)
[![Python 3.11+](https://img.shields.io/badge/Python-3.11+-3776AB?logo=python&logoColor=white)](https://www.python.org/)
[![PRs Welcome](https://img.shields.io/badge/PRs-welcome-brightgreen.svg)](http://makeapullrequest.com)

[Features](#features) • [Quick Start](#quick-start) • [Documentation](#usage-examples) • [API](#api-reference) • [Contributing](#contributing)

</div>

---

## Table of Contents

- [Overview](#overview)
- [Features](#features)
- [Architecture](#architecture)
- [Getting Started](#quick-start)
- [Usage Examples](#usage-examples)
- [API Reference](#api-reference)
- [Configuration](#configuration)
- [Testing](#testing)
- [Performance](#performance)
- [Troubleshooting](#troubleshooting)
- [Contributing](#contributing)
- [License](#license)
- [Community](#community)

---

## Overview

Quant Enthusiasts Risk Engine is a cross-language quantitative finance platform for portfolio risk management and options pricing. Built with C++17, wrapped with Python bindings, and featuring an interactive web dashboard.

### Key Capabilities

- High-performance C++ core optimized for low-latency calculations
- Modular design allowing independent component usage
- Cross-platform support (Windows, Linux, macOS)
- Multiple pricing models (Black-Scholes, Binomial Tree, Jump Diffusion)
- RESTful API with interactive web dashboard
- Comprehensive test suite with full core coverage

---

## Features

### Pricing Models

| Model | Type | Options Supported | Key Features |
|-------|------|-------------------|--------------|
| **Black-Scholes** | Analytical | European | Fast, Greeks calculation |
| **Binomial Tree** | Numerical | European & American | Early exercise, configurable steps |
| **Merton Jump Diffusion** | Analytical | European | Discontinuous jumps, realistic dynamics |

### Risk Analytics

- **Greeks Calculation**: Delta, Gamma, Vega, Theta, Rho
- **Value at Risk (VaR)**: Monte Carlo simulation with 100K+ paths
- **Portfolio Analytics**: Net positions, PV aggregation, exposure analysis
- **Implied Volatility**: Surface construction and interpolation

### Technical Capabilities

- Real-time portfolio risk calculation
- Multi-asset portfolio support
- Configurable Monte Carlo simulations
- Comprehensive input validation
- RESTful API with JSON interface
- Interactive web dashboard

---

## Architecture

```
┌─────────────────────────────────────────────────────────────┐
│                     JS Dashboard (Frontend)                  │
│                 Interactive Portfolio Builder                │
└─────────────────────┬───────────────────────────────────────┘
                      │ HTTP/JSON
┌─────────────────────▼───────────────────────────────────────┐
│                   Python API Layer (Flask)                   │
│              RESTful Endpoints + Validation                  │
└─────────────────────┬───────────────────────────────────────┘
                      │ pybind11
┌─────────────────────▼───────────────────────────────────────┐
│                  C++ Risk Engine (Core)                      │
│     High-Performance Calculations + Pricing Models           │
└─────────────────────────────────────────────────────────────┘
```

### Project Structure

```
Quant-Enthusiasts-Risk-Engine/
├── cpp_engine/                 # C++ core engine
│   ├── src/
│   │   ├── utils/              # Pricing models
│   │   │   ├── BlackScholes.*
│   │   │   ├── BinomialTree.*
│   │   │   ├── JumpDiffusion.*
│   │   │   └── ImpliedVolatilitySurface.*
│   │   ├── Instrument.*        # Option instruments
│   │   ├── MarketData.*        # Market data handling
│   │   ├── Portfolio.*         # Portfolio management
│   │   └── RiskEngine.*        # Risk calculations
│   ├── build.sh                # Build automation
│   └── CMakeLists.txt
├── python_api/                 # Python bindings + API
│   ├── app.py                  # Flask server
│   ├── pybind_wrapper.cpp      # Python bindings
│   ├── requirements.txt
│   └── setup.py
└── js_dashboard/               # Web interface
    └── index.html              # Single-page application
```

---

## Quick Start

### Prerequisites

```bash
# Required
- C++17 compiler (GCC 7+, Clang 5+, MSVC 2017+)
- Python 3.11 or higher
- CMake 3.25+
- pip (Python package manager)

# Optional
- Node.js (for dashboard serving)
```

### Installation

#### 1. Clone the Repository

```bash
git clone https://github.com/Quant-Enthusiasts/Quant-Enthusiasts-Risk-Engine.git
cd Quant-Enthusiasts-Risk-Engine
```

#### 2. Build C++ Engine

**Note**: On Linux/macOS, comment out `#include <corecrt_math_defines.h>` in source files (Windows-only header).
Make sure to convert the build.sh file from CRLF line-ending to LF as well for it to work properly.

**Quick Build (Recommended)**

```bash
cd cpp_engine
chmod +x build.sh
./build.sh --all    # Clean, build, and test
```

**Manual Build**

```bash
mkdir -p cpp_engine/build && cd cpp_engine/build
cmake ..
cmake --build .
./risk-engine
```

**Build Options**

```bash
./build.sh                      # Clean + build
./build.sh --compiler clang     # Use Clang compiler
./build.sh --test               # Run all tests
./build.sh --test blackscholes  # Run specific test
./build.sh --clean              # Clean only
./build.sh --help               # Show help
```

#### 3. Setup Python API

```bash
cd python_api

# Create virtual environment
python -m venv venv
source venv/bin/activate        # Linux/Mac
# venv\Scripts\activate         # Windows

# Install dependencies
pip install -r requirements.txt

# Build Python bindings
python setup.py build_ext --inplace

# Start API server
python app.py
```

Server runs at: `http://127.0.0.1:5000`

#### 4. Launch Dashboard

```bash
cd js_dashboard

# Option 1: Direct open (may have CORS issues)
open index.html

# Option 2: Local server (recommended)
npx serve .
# Navigate to http://localhost:3000
```

---

## Usage Examples

### Example 1: C++ Standalone Engine

```bash
cd cpp_engine/build
./risk-engine
```

Output includes portfolio risk analysis, model comparisons, American options pricing, Greeks calculations, and VaR metrics.

### Example 2: Python API - Price Single Option

```bash
curl -X POST http://127.0.0.1:5000/price_option \
  -H "Content-Type: application/json" \
  -d '{
    "type": "call",
    "strike": 100,
    "expiry": 1.0,
    "asset_id": "AAPL",
    "style": "european",
    "pricing_model": "blackscholes",
    "market_data": {
      "spot": 105,
      "rate": 0.05,
      "vol": 0.25
    }
  }'
```

**Response:**
```json
{
  "price": 12.34,
  "greeks": {
    "delta": 0.6368,
    "gamma": 0.0178,
    "vega": 37.45,
    "theta": -6.42,
    "rho": 48.23
  }
}
```

### Example 3: Portfolio Risk Calculation

```bash
curl -X POST http://127.0.0.1:5000/calculate_risk \
  -H "Content-Type: application/json" \
  -d '{
    "portfolio": [
      {
        "type": "call",
        "strike": 100,
        "expiry": 1.0,
        "asset_id": "AAPL",
        "quantity": 100,
        "style": "european"
      },
      {
        "type": "put",
        "strike": 95,
        "expiry": 1.0,
        "asset_id": "AAPL",
        "quantity": -50,
        "style": "european"
      }
    ],
    "market_data": {
      "AAPL": {
        "spot": 105,
        "rate": 0.05,
        "vol": 0.25
      }
    },
    "var_parameters": {
      "simulations": 100000,
      "confidence": 0.95,
      "time_horizon": 1.0
    }
  }'
```

### Example 4: Web Dashboard Workflow

1. Start API: `python python_api/app.py`
2. Open Dashboard: `js_dashboard/index.html`
3. Define Market Data: Add assets (AAPL, MSFT, etc.) with spot/rate/vol
4. Build Portfolio: Add options with strikes, expiries, quantities
5. Calculate Risk: View PV, Greeks, VaR in real-time

---

## API Reference

### Base URL

```
http://127.0.0.1:5000
```

### Endpoints

#### Health Check

```http
GET /health
```

**Response:**
```json
{
  "status": "healthy",
  "version": "1.0.0",
  "timestamp": "2025-10-20T10:30:00Z"
}
```

#### Price Option

```http
POST /price_option
```

**Request Body:**
```json
{
  "type": "call" | "put",
  "strike": number,
  "expiry": number,
  "asset_id": string,
  "style": "european" | "american",
  "pricing_model": "blackscholes" | "binomial" | "jumpdiffusion",
  "binomial_steps": number (optional, default: 100),
  "jump_parameters": {
    "lambda": number,
    "mean": number,
    "vol": number
  } (optional),
  "market_data": {
    "spot": number,
    "rate": number,
    "vol": number
  }
}
```

#### Calculate Portfolio Risk

```http
POST /calculate_risk
```

**Request Body:**
```json
{
  "portfolio": [
    {
      "type": "call" | "put",
      "strike": number,
      "expiry": number,
      "asset_id": string,
      "quantity": number,
      "style": "european" | "american"
    }
  ],
  "market_data": {
    "<asset_id>": {
      "spot": number,
      "rate": number,
      "vol": number
    }
  },
  "var_parameters": {
    "simulations": number (default: 100000),
    "confidence": number (default: 0.95),
    "time_horizon": number (default: 1.0),
    "seed": number (optional)
  }
}
```

#### Portfolio Net Position

```http
POST /portfolio/net_position/<asset_id>
```

#### Portfolio Summary

```http
POST /portfolio/summary
```

---

## Configuration

### VaR Parameters

| Parameter | Type | Default | Description |
|-----------|------|---------|-------------|
| `simulations` | int | 100000 | Number of Monte Carlo paths |
| `confidence` | float | 0.95 | Confidence level (0-1) |
| `time_horizon` | float | 1.0 | Time horizon in years |
| `seed` | int | random | Random seed for reproducibility |

### Option Parameters

| Parameter | Type | Required | Description |
|-----------|------|----------|-------------|
| `type` | string | Yes | "call" or "put" |
| `strike` | float | Yes | Strike price |
| `expiry` | float | Yes | Time to expiry (years) |
| `asset_id` | string | Yes | Asset identifier |
| `quantity` | int | Yes | Position size (+ long, - short) |
| `style` | string | Yes | "european" or "american" |
| `pricing_model` | string | No | Default: "blackscholes" |

### Jump Diffusion Parameters

```json
{
  "lambda": 2.0,      // Jump intensity (jumps per year)
  "mean": -0.05,      // Mean jump size
  "vol": 0.15         // Jump size volatility
}
```

---

## Testing

### Test Coverage

- Black-Scholes pricing and Greeks
- Binomial tree (American/European)
- Jump diffusion model
- Portfolio aggregation
- Risk engine VaR calculations

### Run Tests

```bash
# All tests
cd cpp_engine
./build.sh --test

# Specific test suites
./build.sh --test blackscholes
./build.sh --test binomial
./build.sh --test portfolio
./build.sh --test risk_engine
```

### Test Output

```
[==========] Running 15 tests from 5 test suites.
[----------] 3 tests from BlackScholesTest
[ RUN      ] BlackScholesTest.CallPrice
[       OK ] BlackScholesTest.CallPrice (0 ms)
...
[==========] 15 tests from 5 test suites ran. (50 ms total)
[  PASSED  ] 15 tests.
```

---

## Performance

### Benchmarks

| Operation | Time | Notes |
|-----------|------|-------|
| Single option pricing | < 1 μs | Black-Scholes |
| Portfolio (100 options) | < 1 ms | Full Greeks |
| VaR (100K simulations) | < 1 s | 95% confidence |
| Binomial tree (1000 steps) | < 10 ms | American option |

### Optimization Tips

- Use C++ engine directly for maximum performance
- Batch API requests when possible
- Reduce VaR simulations for quick estimates
- Cache market data for repeated calculations

### Scalability

- **Tested Portfolio Size**: 1,000+ instruments
- **Concurrent API Requests**: 50+ simultaneous
- **Memory Usage**: ~100 MB for typical portfolio
- **CPU Utilization**: Scales with available cores

---

## Troubleshooting

### Common Issues

<details>
<summary><b>CMake cannot find compiler</b></summary>

```bash
# Specify compiler explicitly
cmake -DCMAKE_CXX_COMPILER=g++ ..

# Or use Clang
cmake -DCMAKE_CXX_COMPILER=clang++ ..
```
</details>

<details>
<summary><b>Math constants not defined (Linux/macOS)</b></summary>

Comment out this line in source files:
```cpp
// #include <corecrt_math_defines.h>  // Windows only
```
</details>

<details>
<summary><b>Python module 'quant_risk_engine' not found</b></summary>

```bash
cd python_api
python setup.py build_ext --inplace
# Verify build succeeded
ls -la *.so  # Linux/Mac
ls -la *.pyd # Windows
```
</details>

<details>
<summary><b>API returns "Connection refused"</b></summary>

1. Check Flask server is running: `python python_api/app.py`
2. Verify port 5000 is available: `lsof -i :5000` (Linux/Mac)
3. Check firewall settings
4. Try `curl http://127.0.0.1:5000/health`
</details>

<details>
<summary><b>Dashboard CORS errors</b></summary>

Use a local server instead of opening HTML directly:
```bash
cd js_dashboard
npx serve .
# Or
python -m http.server 8000
```
</details>

<details>
<summary><b>Build fails with "C++17 required"</b></summary>

Update compiler or specify standard:
```bash
cmake -DCMAKE_CXX_STANDARD=17 ..
```
</details>

---

## Contributing

### How to Contribute

1. Fork the repository
2. Create a feature branch
   ```bash
   git checkout -b feature/amazing-feature
   ```
3. Commit your changes
   ```bash
   git commit -m 'Add amazing feature'
   ```
4. Push to your branch
   ```bash
   git push origin feature/amazing-feature
   ```
5. Open a Pull Request

### Contribution Guidelines

- Follow existing code style
- Add tests for new features
- Update documentation
- Ensure all tests pass
- Write clear commit messages

### Development Setup

```bash
# Install development dependencies
pip install -r requirements-dev.txt

# Run code formatters
clang-format -i cpp_engine/src/**/*.cpp
black python_api/

# Run linters
cppcheck cpp_engine/src/
pylint python_api/app.py
```

---

## License

This project is licensed under the **MIT License**.

```
MIT License

Copyright (c) 2025 Quant Enthusiasts

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
```

See [LICENSE](LICENSE) file for full details.

---

## Community

- **Discord**: [https://discord.com/invite/z3S9Fguzw3](https://discord.com/invite/z3S9Fguzw3)
- **GitHub Issues**: [https://github.com/Quant-Enthusiasts/Quant-Enthusiasts-Risk-Engine/issues](https://github.com/Quant-Enthusiasts/Quant-Enthusiasts-Risk-Engine/issues)

---

<div align="center">

**[⬆ Back to Top](#quant-enthusiasts-risk-engine)**

</div>