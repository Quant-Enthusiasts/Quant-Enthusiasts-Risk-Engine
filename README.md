# Quant Enthusiasts Risk Engine

A high-performance quantitative finance platform for portfolio risk management and options pricing. Built with C++17 core, Python bindings, and an interactive web dashboard.

[![License: MIT](https://img.shields.io/badge/License-MIT-blue.svg)](https://opensource.org/licenses/MIT)
[![C++17](https://img.shields.io/badge/C++-17-00599C?logo=cplusplus)](https://isocpp.org/)
[![Python 3.11+](https://img.shields.io/badge/Python-3.11+-3776AB?logo=python&logoColor=white)](https://www.python.org/)

## Overview

Cross-language quantitative finance platform offering:

- **Multiple Pricing Models**: Black-Scholes, Binomial Tree, Merton Jump Diffusion
- **Risk Analytics**: Greeks calculation, Value at Risk (Monte Carlo), Portfolio aggregation
- **Live Market Data**: Automatic fetching from Yahoo Finance with caching
- **RESTful API**: Flask-based endpoints with comprehensive validation
- **Web Dashboard**: Interactive portfolio builder and risk visualizer

## Quick Start

```bash
# Clone repository
git clone https://github.com/Quant-Enthusiasts/Quant-Enthusiasts-Risk-Engine.git
cd Quant-Enthusiasts-Risk-Engine

# Build C++ engine
cd cpp_engine
mkdir build && cd build
cmake .. -GNinja -DCMAKE_BUILD_TYPE=Release
cmake --build .
cmake --install .

# Setup Python API
cd ../../python_api
python3 -m venv venv
source venv/bin/activate
pip install -r requirements.txt
python setup.py build_ext --inplace

# Start API server
python app.py
```

Server runs at `http://127.0.0.1:5000`

For detailed setup instructions, see [INSTALLATION.md](INSTALLATION.md).

## Architecture

```
JavaScript Dashboard (Frontend)
         |
         | HTTP/JSON
         v
  Python API Layer (Flask)
         |
         | pybind11
         v
   C++ Risk Engine (Core)
```

**Project Structure:**
```
Quant-Enthusiasts-Risk-Engine/
├── cpp_engine/          # C++ core with pricing models
├── python_api/          # Flask API + Python bindings
├── js_dashboard/        # Web interface
└── docs/                # Additional documentation
```

## Core Features

### Pricing Models

| Model | Type | Options | Key Features |
|-------|------|---------|--------------|
| Black-Scholes | Analytical | European | Fast, Greeks calculation |
| Binomial Tree | Numerical | European/American | Early exercise, configurable steps |
| Merton Jump Diffusion | Analytical | European | Discontinuous jumps |

### Risk Metrics

- **Greeks**: Delta, Gamma, Vega, Theta, Rho
- **VaR**: Monte Carlo simulation (configurable paths)
- **Expected Shortfall**: 95%/99% confidence levels
- **Portfolio Analytics**: Net positions, PV aggregation

### Market Data Integration

- Automatic fetching from Yahoo Finance
- SQLite-based caching (24-hour expiration)
- Bulk ticker updates
- Volatility calculation from historical data

See [MARKET_DATA.md](MARKET_DATA.md) for details.

## Usage Example

```bash
# Fetch market data
curl -X POST http://127.0.0.1:5000/update_market_data \
  -H "Content-Type: application/json" \
  -d '{"tickers": ["AAPL", "GOOGL"]}'

# Calculate portfolio risk (auto-fetches missing data)
curl -X POST http://127.0.0.1:5000/calculate_risk \
  -H "Content-Type: application/json" \
  -d '{
    "portfolio": [
      {
        "type": "call",
        "strike": 180,
        "expiry": 1.0,
        "asset_id": "AAPL",
        "quantity": 100,
        "style": "european"
      }
    ],
    "market_data": {}
  }'
```

Full API documentation: [API.md](API.md)

## Performance

| Operation | Latency | Notes |
|-----------|---------|-------|
| Single option pricing | < 1 microsecond | Black-Scholes |
| Portfolio (100 options) | < 1 ms | Full Greeks |
| VaR (100K simulations) | < 1 s | 95% confidence |

## Documentation

- [Installation Guide](INSTALLATION.md) - Detailed setup for all platforms
- [API Reference](API.md) - Complete endpoint documentation
- [Development Guide](DEVELOPMENT.md) - Testing, contributing, CI/CD
- [Market Data Guide](MARKET_DATA.md) - YFinance integration details
- [Deployment Guide](DEPLOYMENT.md) - Docker, production setup

## Contributing

We welcome contributions! Please see [DEVELOPMENT.md](DEVELOPMENT.md) for:

- Development setup
- Code style guidelines
- Testing requirements
- Pull request process

## Support

- **GitHub Issues**: [Report bugs or request features](https://github.com/Quant-Enthusiasts/Quant-Enthusiasts-Risk-Engine/issues)
- **Discord**: [Join our community](https://discord.com/invite/z3S9Fguzw3)

## License

This project is licensed under the MIT License. See [LICENSE](LICENSE) for details.

---

**Made by Quant Enthusiasts** | [Documentation](docs/) | [GitHub](https://github.com/Quant-Enthusiasts)
