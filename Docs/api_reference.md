# API Reference

Complete REST API documentation for the Quant Enthusiasts Risk Engine.

## Base URL

```
http://127.0.0.1:5000
```

## Authentication

Currently no authentication required. For production deployment, implement API keys or OAuth2.

## Response Format

All responses are JSON with the following structure:

**Success:**
```json
{
  "result_field": "value",
  "...": "..."
}
```

**Error:**
```json
{
  "error": "Error message description"
}
```

## Endpoints

### Health Check

Check API status and available features.

**Request:**
```http
GET /health
```

**Response (200):**
```json
{
  "status": "healthy",
  "service": "quant-risk-engine",
  "version": "3.1",
  "features": [
    "european_options",
    "american_options",
    "multiple_pricing_models",
    "portfolio_analytics",
    "var_calculation",
    "live_market_data"
  ],
  "cache_info": {
    "cached_assets": 5,
    "cache_location": "market_data_cache.db"
  }
}
```

---

### Price Single Option

Calculate option price and Greeks for a single instrument.

**Request:**
```http
POST /price_option
Content-Type: application/json
```

**Body:**
```json
{
  "type": "call",
  "strike": 100.0,
  "expiry": 1.0,
  "asset_id": "AAPL",
  "style": "european",
  "pricing_model": "blackscholes",
  "market_data": {
    "spot": 105.0,
    "rate": 0.05,
    "vol": 0.25,
    "dividend": 0.01
  }
}
```

**Parameters:**

| Field | Type | Required | Description |
|-------|------|----------|-------------|
| `type` | string | Yes | "call" or "put" |
| `strike` | float | Yes | Strike price |
| `expiry` | float | Yes | Time to expiry (years) |
| `asset_id` | string | Yes | Asset identifier |
| `style` | string | No | "european" or "american" (default: "european") |
| `pricing_model` | string | No | "blackscholes", "binomial", "jumpdiffusion" (default: "blackscholes") |
| `binomial_steps` | int | No | Number of steps for binomial tree (default: 100) |
| `jump_parameters` | object | No | Jump diffusion parameters (see below) |
| `market_data` | object | No | If omitted, auto-fetches from cache/YFinance |

**Jump Parameters:**
```json
{
  "lambda": 2.0,    // Jump intensity (jumps per year)
  "mean": -0.05,    // Mean jump size
  "vol": 0.15       // Jump size volatility
}
```

**Market Data:**
```json
{
  "spot": 105.0,      // Current spot price
  "rate": 0.05,       // Risk-free rate (annualized)
  "vol": 0.25,        // Volatility (annualized)
  "dividend": 0.01    // Dividend yield (optional, default: 0.0)
}
```

**Response (200):**
```json
{
  "price": 12.34,
  "delta": 0.6368,
  "gamma": 0.0178,
  "vega": 37.45,
  "theta": -6.42,
  "rho": 48.23,
  "instrument_type": "EuropeanOption",
  "market_data_auto_fetched": false,
  "market_data_used": {
    "spot": 105.0,
    "rate": 0.05,
    "vol": 0.25,
    "dividend": 0.01
  }
}
```

**Errors:**
- `400`: Validation error (missing fields, invalid values)
- `500`: Runtime error (pricing calculation failed)

---

### Calculate Portfolio Risk

Calculate comprehensive risk metrics for a portfolio of options.

**Request:**
```http
POST /calculate_risk
Content-Type: application/json
```

**Body:**
```json
{
  "portfolio": [
    {
      "type": "call",
      "strike": 100.0,
      "expiry": 1.0,
      "asset_id": "AAPL",
      "quantity": 100,
      "style": "european"
    },
    {
      "type": "put",
      "strike": 95.0,
      "expiry": 1.0,
      "asset_id": "AAPL",
      "quantity": -50,
      "style": "european"
    }
  ],
  "market_data": {
    "AAPL": {
      "spot": 105.0,
      "rate": 0.05,
      "vol": 0.25
    }
  },
  "var_parameters": {
    "simulations": 100000,
    "confidence": 0.95,
    "time_horizon": 1.0,
    "seed": 42
  }
}
```

**Parameters:**

| Field | Type | Required | Description |
|-------|------|----------|-------------|
| `portfolio` | array | Yes | Array of option specifications |
| `market_data` | object | No | Market data by asset (auto-fetches if missing) |
| `var_parameters` | object | No | VaR simulation configuration |

**Portfolio Item:**
```json
{
  "type": "call",           // "call" or "put"
  "strike": 100.0,          // Strike price
  "expiry": 1.0,            // Time to expiry (years)
  "asset_id": "AAPL",       // Asset identifier
  "quantity": 100,          // Position size (positive = long, negative = short)
  "style": "european",      // "european" or "american"
  "pricing_model": "blackscholes"  // Optional: pricing model
}
```

**VaR Parameters:**
```json
{
  "simulations": 100000,    // Number of Monte Carlo paths (max: 1,000,000)
  "confidence": 0.95,       // Confidence level (0-1)
  "time_horizon": 1.0,      // Time horizon in days
  "seed": 42                // Random seed for reproducibility (optional)
}
```

**Response (200):**
```json
{
  "total_pv": 123456.78,
  "total_delta": 0.6543,
  "total_gamma": 0.0234,
  "total_vega": 456.78,
  "total_theta": -12.34,
  "value_at_risk_95": -5678.90,
  "value_at_risk_99": -7890.12,
  "expected_shortfall_95": -6543.21,
  "expected_shortfall_99": -8901.23,
  "portfolio_size": 2,
  "var_parameters": {
    "simulations": 100000,
    "confidence_level": 0.95,
    "time_horizon_days": 1.0
  },
  "market_data_info": {
    "auto_fetched_assets": [],
    "market_data_used": {
      "AAPL": {
        "spot": 105.0,
        "rate": 0.05,
        "vol": 0.25,
        "dividend": 0.0
      }
    }
  }
}
```

**Errors:**
- `400`: Validation error (invalid portfolio, market data missing)
- `500`: Runtime error (risk calculation failed)

---

### Portfolio Net Position

Get net position for a specific asset across portfolio.

**Request:**
```http
POST /portfolio/net_position/<asset_id>
Content-Type: application/json
```

**Body:**
```json
{
  "portfolio": [
    {"type": "call", "strike": 100, "expiry": 1.0, "asset_id": "AAPL", "quantity": 100, "style": "european"},
    {"type": "put", "strike": 95, "expiry": 1.0, "asset_id": "AAPL", "quantity": -50, "style": "european"}
  ]
}
```

**Response (200):**
```json
{
  "asset_id": "AAPL",
  "net_quantity": 50,
  "direction": "long"
}
```

---

### Portfolio Summary

Get portfolio statistics and composition.

**Request:**
```http
POST /portfolio/summary
Content-Type: application/json
```

**Body:**
```json
{
  "portfolio": [...]
}
```

**Response (200):**
```json
{
  "portfolio_size": 5,
  "unique_assets": 3,
  "net_positions": {
    "AAPL": 100,
    "GOOGL": -50,
    "MSFT": 75
  },
  "instrument_counts": {
    "european": 4,
    "american": 1,
    "calls": 3,
    "puts": 2
  }
}
```

---

### Update Market Data

Fetch live market data from Yahoo Finance.

**Request:**
```http
POST /update_market_data
Content-Type: application/json
```

**Body:**
```json
{
  "tickers": ["AAPL", "GOOGL", "MSFT"],
  "force_refresh": false
}
```

**Parameters:**

| Field | Type | Required | Description |
|-------|------|----------|-------------|
| `tickers` | array | Yes | Ticker symbols (max 50) |
| `force_refresh` | boolean | No | Bypass cache (default: false) |

**Response (200):**
```json
{
  "success": true,
  "updated": {
    "AAPL": {
      "asset_id": "AAPL",
      "spot": 175.43,
      "vol": 0.2847,
      "rate": 0.0445,
      "dividend": 0.0052,
      "last_updated": "2025-10-25T14:30:00.123456",
      "source": "yfinance"
    }
  },
  "failed": [],
  "summary": {
    "total_requested": 3,
    "successful": 3,
    "failed": 0
  },
  "timestamp": "2025-10-25T14:30:00.123456"
}
```

**Response (207 Multi-Status):**
Partial success when some tickers fail:
```json
{
  "success": false,
  "updated": {...},
  "failed": [
    {
      "ticker": "INVALID",
      "error": "Failed to fetch data for INVALID: No price data available"
    }
  ],
  "summary": {...}
}
```

**Errors:**
- `400`: Invalid request (empty tickers, too many, invalid format)

---

### Get Cached Market Data

Retrieve cached market data without fetching new data.

**Request:**
```http
GET /get_cached_market_data?asset_id=AAPL
```

**Query Parameters:**

| Parameter | Type | Required | Description |
|-----------|------|----------|-------------|
| `asset_id` | string | No | Specific asset (omit for all) |

**Response (200):**
```json
{
  "AAPL": {
    "spot": 175.43,
    "vol": 0.2847,
    "rate": 0.0445,
    "dividend": 0.0052,
    "last_updated": "2025-10-25T14:30:00.123456",
    "source": "yfinance"
  }
}
```

**Errors:**
- `404`: Asset not found in cache

---

### Clear Market Data Cache

Delete all cached market data.

**Request:**
```http
DELETE /clear_market_data_cache
```

**Response (200):**
```json
{
  "success": true,
  "message": "Market data cache cleared",
  "timestamp": "2025-10-25T14:35:00.123456"
}
```

---

## Error Handling

All endpoints follow consistent error response format:

**Validation Error (400):**
```json
{
  "error": "Validation error: Portfolio item 0: missing required field 'strike'"
}
```

**Runtime Error (500):**
```json
{
  "error": "Runtime error: Risk calculation produced invalid results"
}
```

**Not Found (404):**
```json
{
  "error": "Endpoint not found"
}
```

## Rate Limiting

Currently no rate limiting. For production deployment:
- Implement per-IP rate limiting
- Use Redis for distributed rate limiting
- Add API keys with tiered limits

## Examples

### Complete Workflow

```bash
# 1. Fetch market data
curl -X POST http://127.0.0.1:5000/update_market_data \
  -H "Content-Type: application/json" \
  -d '{"tickers": ["AAPL", "GOOGL"]}'

# 2. Price single option
curl -X POST http://127.0.0.1:5000/price_option \
  -H "Content-Type: application/json" \
  -d '{
    "type": "call",
    "strike": 180,
    "expiry": 1.0,
    "asset_id": "AAPL",
    "style": "european",
    "market_data": {}
  }'

# 3. Calculate portfolio risk
curl -X POST http://127.0.0.1:5000/calculate_risk \
  -H "Content-Type: application/json" \
  -d '{
    "portfolio": [
      {"type": "call", "strike": 180, "expiry": 1.0, "asset_id": "AAPL", "quantity": 100, "style": "european"},
      {"type": "put", "strike": 140, "expiry": 0.5, "asset_id": "GOOGL", "quantity": -50, "style": "european"}
    ],
    "market_data": {},
    "var_parameters": {"simulations": 100000}
  }'
```

## Client Libraries

Coming soon:
- Python SDK
- JavaScript/TypeScript SDK
- R package

## WebSocket Support

Planned for real-time updates:
- Live portfolio valuations
- Streaming market data
- Risk metric updates

See [DEVELOPMENT.md](DEVELOPMENT.md) for contributing to these features.