# Quick Start Guide

Get the Quant Enthusiasts Risk Engine running in under 10 minutes.

## Prerequisites Check

Verify you have the required tools:

```bash
# Check versions
python3 --version    # Need 3.11+
cmake --version      # Need 3.25+
g++ --version        # Need 7+ (or clang 5+)
```

If any are missing, see [INSTALLATION.md](INSTALLATION.md) for setup instructions.

## 5-Minute Setup

### 1. Clone and Build (3 minutes)

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
cd ../..
```

### 2. Setup Python API (2 minutes)

```bash
# Create virtual environment
cd python_api
python3 -m venv venv
source venv/bin/activate  # Windows: venv\Scripts\activate

# Install dependencies
pip install -r requirements.txt
python setup.py build_ext --inplace
```

### 3. Start Server

```bash
# Set module path
export PYTHONPATH="../cpp_engine/install/lib:$PYTHONPATH"

# Start API
python app.py
```

Server is now running at `http://127.0.0.1:5000`

## Test Your Installation

Open a new terminal and run:

```bash
# Health check
curl http://127.0.0.1:5000/health

# Price an option
curl -X POST http://127.0.0.1:5000/price_option \
  -H "Content-Type: application/json" \
  -d '{
    "type": "call",
    "strike": 100,
    "expiry": 1.0,
    "asset_id": "AAPL",
    "style": "european",
    "market_data": {
      "spot": 105,
      "rate": 0.05,
      "vol": 0.25
    }
  }'
```

Expected output:
```json
{
  "price": 12.34,
  "delta": 0.6368,
  "gamma": 0.0178,
  "vega": 37.45,
  "theta": -6.42
}
```

## Common First-Time Issues

### "ModuleNotFoundError: quant_risk_engine"

**Fix**: Set PYTHONPATH before running Flask
```bash
export PYTHONPATH="$PWD/../cpp_engine/install/lib:$PYTHONPATH"
```

### "Port 5000 already in use" (macOS)

**Fix**: Use alternative port
```bash
python -m flask --app app run --port 5050
```

### "Permission denied: build.sh"

**Fix**: Make script executable
```bash
chmod +x cpp_engine/build.sh
```

## Quick Examples

### Example 1: Fetch Live Market Data

```bash
curl -X POST http://127.0.0.1:5000/update_market_data \
  -H "Content-Type: application/json" \
  -d '{"tickers": ["AAPL", "GOOGL"]}'
```

### Example 2: Calculate Portfolio Risk

```bash
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
    "market_data": {},
    "var_parameters": {
      "simulations": 10000,
      "confidence": 0.95
    }
  }'
```

### Example 3: Use the Dashboard

```bash
# In a new terminal
cd js_dashboard
npx serve .
# Open http://localhost:3000 in browser
```

## Next Steps

### Learn More

- **API Reference**: See [API.md](API.md) for all endpoints
- **Market Data**: Read [MARKET_DATA.md](MARKET_DATA.md) for YFinance integration
- **Development**: Check [DEVELOPMENT.md](DEVELOPMENT.md) to contribute

### Try These Features

1. **American Options**: Set `"style": "american"` in request
2. **Jump Diffusion**: Use `"pricing_model": "jumpdiffusion"`
3. **VaR Calculation**: Add `"var_parameters"` to risk request
4. **Greeks**: All option pricing returns full Greeks

### Common Workflows

**Daily Portfolio Risk Check**:
```bash
# 1. Update market data
curl -X POST http://localhost:5000/update_market_data \
  -d '{"tickers": ["AAPL", "GOOGL", "MSFT"]}'

# 2. Calculate risk (auto-uses cached data)
curl -X POST http://localhost:5000/calculate_risk \
  -d '{"portfolio": [...], "market_data": {}}'
```

**Compare Pricing Models**:
```bash
# Black-Scholes
curl -X POST http://localhost:5000/price_option \
  -d '{..., "pricing_model": "blackscholes"}'

# Binomial Tree
curl -X POST http://localhost:5000/price_option \
  -d '{..., "pricing_model": "binomial", "binomial_steps": 1000}'

# Jump Diffusion
curl -X POST http://localhost:5000/price_option \
  -d '{..., "pricing_model": "jumpdiffusion", "jump_parameters": {...}}'
```

## Troubleshooting

### Build Fails

**Clean and rebuild**:
```bash
cd cpp_engine
rm -rf build install
mkdir build && cd build
cmake .. -GNinja -DCMAKE_BUILD_TYPE=Release
cmake --build .
cmake --install .
```

### Python Module Not Found

**Verify build output**:
```bash
ls -la cpp_engine/install/lib/
# Should see: quant_risk_engine.cpython-*.so
```

### API Returns Errors

**Check logs**:
```bash
# Start Flask in debug mode
python -m flask --app app run --debug
```

## Getting Help

- **Documentation**: Check [INSTALLATION.md](INSTALLATION.md) for detailed setup
- **GitHub Issues**: [Report problems](https://github.com/Quant-Enthusiasts/Quant-Enthusiasts-Risk-Engine/issues)
- **Discord**: [Join community](https://discord.com/invite/z3S9Fguzw3)

## Development Setup

If you want to contribute:

```bash
# Install development dependencies
cd python_api
pip install -r requirements-dev.txt

# Run tests
cd ../cpp_engine/build
ctest --output-on-failure

cd ../../python_api
pytest test_market_data.py -v
```

See [DEVELOPMENT.md](DEVELOPMENT.md) for complete development guide.

---

**Setup complete!** You now have a working quantitative finance risk engine.

Ready to build something? Start with the [API Reference](API.md) or join our [Discord community](https://discord.com/invite/z3S9Fguzw3).