# Market Data Integration

Guide for using live market data fetching with Yahoo Finance integration.

## Overview

The Risk Engine automatically fetches real-time market data using the YFinance library, eliminating manual input requirements for spot prices, volatilities, and risk-free rates.

## Features

- **Automatic Data Fetching**: Current prices, historical volatility, risk-free rates
- **Smart Caching**: SQLite database with 24-hour expiration
- **Bulk Updates**: Fetch multiple tickers in single request
- **Fallback Support**: Graceful degradation when data unavailable
- **Error Handling**: Detailed error messages for invalid tickers

## Quick Start

### 1. Fetch Market Data

```bash
curl -X POST http://127.0.0.1:5000/update_market_data \
  -H "Content-Type: application/json" \
  -d '{"tickers": ["AAPL", "GOOGL", "MSFT"]}'
```

### 2. Use in Risk Calculation

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
    "market_data": {}
  }'
```

**Note**: Empty `market_data` object triggers automatic fetching from cache or YFinance.

## API Endpoints

### Update Market Data

Fetch and cache live market data.

**Endpoint**: `POST /update_market_data`

**Request**:
```json
{
  "tickers": ["AAPL", "GOOGL"],
  "force_refresh": false
}
```

**Parameters**:
- `tickers` (required): Array of ticker symbols (max 50)
- `force_refresh` (optional): Bypass cache (default: false)

**Response (Success)**:
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
    "total_requested": 2,
    "successful": 2,
    "failed": 0
  }
}
```

**Response (Partial Failure - 207)**:
```json
{
  "success": false,
  "updated": {...},
  "failed": [
    {
      "ticker": "INVALID",
      "error": "No price data available"
    }
  ],
  "summary": {
    "total_requested": 2,
    "successful": 1,
    "failed": 1
  }
}
```

### Get Cached Data

Retrieve cached data without fetching.

**Endpoint**: `GET /get_cached_market_data`

**Query Parameters**:
- `asset_id` (optional): Specific asset, omit for all

**Response**:
```json
{
  "AAPL": {
    "spot": 175.43,
    "vol": 0.2847,
    "rate": 0.0445,
    "dividend": 0.0052,
    "last_updated": "2025-10-25T14:30:00",
    "source": "yfinance"
  }
}
```

### Clear Cache

Delete all cached data.

**Endpoint**: `DELETE /clear_market_data_cache`

**Response**:
```json
{
  "success": true,
  "message": "Market data cache cleared",
  "timestamp": "2025-10-25T14:35:00"
}
```

## Data Sources

### Spot Price

- Current market price from Yahoo Finance
- Falls back to last closing price if market closed
- Validates price is positive

### Volatility

- Calculated from 252 days (1 year) historical data
- Uses log returns: `std(log(P_t / P_{t-1})) * sqrt(252)`
- Default: 0.25 (25%) if insufficient data
- Capped between 1% and 200%

### Risk-Free Rate

- Attempts to fetch 10-year US Treasury rate (^TNX)
- Default: 0.045 (4.5%) if unavailable
- Updated daily

### Dividend Yield

- Annual dividend yield from Yahoo Finance
- Default: 0.0 if not available
- Expressed as decimal (e.g., 0.0052 = 0.52%)

## Caching

### Cache Structure

SQLite database: `python_api/market_data_cache.db`

**Schema**:
```sql
CREATE TABLE market_data (
    asset_id TEXT PRIMARY KEY,
    spot_price REAL NOT NULL,
    volatility REAL NOT NULL,
    risk_free_rate REAL NOT NULL,
    dividend_yield REAL DEFAULT 0.0,
    last_updated TIMESTAMP NOT NULL,
    data_source TEXT DEFAULT 'yfinance'
);
```

### Cache Behavior

**Cache Hit** (data age < 24 hours):
- Returns cached data immediately
- No network request

**Cache Miss** (no data or expired):
- Fetches from YFinance
- Stores in cache
- Returns fresh data

**Force Refresh** (`force_refresh: true`):
- Always fetches from YFinance
- Overwrites cache
- Bypasses age check

### Configuration

**Environment Variables**:
```bash
MARKET_DATA_CACHE_HOURS=24          # Cache expiration
DEFAULT_RISK_FREE_RATE=0.045        # Fallback rate
MARKET_DATA_CACHE_PATH=./market_data_cache.db  # DB location
```

**Python Configuration**:
```python
from market_data_fetcher import MarketDataFetcher, MarketDataCache

# Custom cache location
cache = MarketDataCache(db_path="custom_cache.db")
fetcher = MarketDataFetcher(cache=cache)

# Custom expiration
data = cache.get('AAPL', max_age_hours=12)
```

## Usage Patterns

### Pattern 1: Daily Cache Warming

Pre-populate cache before market open:

```bash
# Cron job: 9:00 AM daily
0 9 * * 1-5 curl -X POST http://localhost:5000/update_market_data \
  -H "Content-Type: application/json" \
  -d '{"tickers": ["AAPL", "GOOGL", "MSFT"], "force_refresh": true}'
```

### Pattern 2: On-Demand Fetching

Let API automatically fetch as needed:

```python
import requests

# No market data provided
response = requests.post('http://localhost:5000/calculate_risk',
    json={
        'portfolio': [...],
        'market_data': {}  # Will auto-fetch
    })
```

### Pattern 3: Hybrid Approach

Provide some data, auto-fetch rest:

```python
response = requests.post('http://localhost:5000/calculate_risk',
    json={
        'portfolio': [
            {'asset_id': 'AAPL', ...},
            {'asset_id': 'GOOGL', ...}
        ],
        'market_data': {
            'AAPL': {  # Manual data for AAPL
                'spot': 180.0,
                'rate': 0.05,
                'vol': 0.28
            }
            # GOOGL will be auto-fetched
        }
    })
```

### Pattern 4: Bulk Portfolio Update

```python
import requests

# Get all unique tickers from portfolio
tickers = ['AAPL', 'GOOGL', 'MSFT', 'AMZN', 'TSLA']

# Fetch all at once
requests.post('http://localhost:5000/update_market_data',
    json={'tickers': tickers, 'force_refresh': True})

# Calculate risk (uses cached data)
requests.post('http://localhost:5000/calculate_risk',
    json={'portfolio': [...], 'market_data': {}})
```

## Error Handling

### Common Errors

| Error | Cause | Solution |
|-------|-------|----------|
| "No price data available" | Invalid/delisted ticker | Verify on finance.yahoo.com |
| "Connection timeout" | Network issues | Check connectivity, retry |
| "Too many requests" | Rate limiting | Batch requests, use caching |
| "Insufficient historical data" | Recent IPO | Manually specify volatility |

### Error Response Examples

**Invalid Ticker**:
```json
{
  "failed": [
    {
      "ticker": "INVALIDXYZ",
      "error": "Failed to fetch data for INVALIDXYZ: No price data available"
    }
  ]
}
```

**Network Error**:
```json
{
  "error": "Failed to fetch data for AAPL: Connection timeout"
}
```

### Retry Logic

```python
import requests
import time

def safe_fetch(tickers, max_retries=3):
    """Fetch market data with exponential backoff"""
    for attempt in range(max_retries):
        try:
            response = requests.post(
                'http://localhost:5000/update_market_data',
                json={'tickers': tickers},
                timeout=30
            )
            
            if response.status_code in [200, 207]:
                return response.json()
            
            if response.status_code >= 500:
                # Server error, retry
                time.sleep(2 ** attempt)
                continue
            else:
                # Client error, don't retry
                return response.json()
                
        except requests.Timeout:
            if attempt < max_retries - 1:
                time.sleep(2 ** attempt)
            else:
                raise
    
    raise Exception("Max retries exceeded")
```

## Performance

### Benchmarks

| Operation | Latency | Notes |
|-----------|---------|-------|
| Cache read | < 1 ms | SQLite SELECT |
| Cache write | < 5 ms | SQLite INSERT |
| Single ticker fetch | 1-3 s | Network dependent |
| 10 tickers fetch | 5-10 s | Sequential |
| Volatility calculation | 10-50 ms | 252 data points |

### Optimization Tips

1. **Batch Requests**
   ```python
   # Good: Single request for multiple tickers
   fetch(['AAPL', 'GOOGL', 'MSFT'])
   
   # Bad: Multiple single-ticker requests
   for ticker in ['AAPL', 'GOOGL', 'MSFT']:
       fetch([ticker])
   ```

2. **Use Caching**
   ```python
   # Check cache first
   cached = get_cached_data('AAPL')
   if not cached or is_stale(cached):
       fetch(['AAPL'])
   ```

3. **Schedule Updates**
   ```python
   # Pre-market data fetch
   schedule.every().day.at("08:30").do(update_portfolio_data)
   ```

## Limitations

- **Data Quality**: Dependent on Yahoo Finance accuracy
- **Rate Limiting**: Yahoo Finance may throttle requests
- **Market Hours**: Real-time data only during trading
- **Geographic Coverage**: Primarily US markets
- **Historical Data**: Limited to YFinance availability

## Advanced Usage

### Custom Data Sources

Extend `MarketDataFetcher` for alternative sources:

```python
from market_data_fetcher import MarketDataFetcher

class AlphaVantageFetcher(MarketDataFetcher):
    def fetch_single(self, ticker, force_refresh=False):
        # Custom implementation
        api_key = os.getenv('ALPHAVANTAGE_API_KEY')
        url = f'https://www.alphavantage.co/query?...'
        # ... fetch and parse
        return {
            'asset_id': ticker,
            'spot': spot_price,
            'vol': volatility,
            'rate': risk_free_rate,
            'dividend': dividend_yield
        }
```

### Volatility Surface Construction

Coming soon:
- Implied volatility from options data
- Volatility smile/skew modeling
- SABR model calibration

### Multi-Currency Support

Planned feature:
- FX rate fetching
- Currency conversion
- Multi-currency portfolios

## Testing

### Unit Tests

```bash
cd python_api
pytest test_market_data.py -v
```

**Test Coverage**:
- Cache CRUD operations
- Data fetching and validation
- Error handling
- API endpoint functionality
- Performance benchmarks

### Integration Tests

```bash
# Start API server
python app.py &

# Test endpoints
pytest test_market_data.py::TestAPIEndpoints -v

# Cleanup
pkill -f "python app.py"
```

### Manual Testing

```bash
# Test basic fetch
curl -X POST http://localhost:5000/update_market_data \
  -d '{"tickers": ["AAPL"]}'

# Verify cache
sqlite3 market_data_cache.db "SELECT * FROM market_data WHERE asset_id='AAPL';"

# Test invalid ticker
curl -X POST http://localhost:5000/update_market_data \
  -d '{"tickers": ["INVALIDXYZ"]}'
```

## Troubleshooting

### Cache Issues

**Problem**: Old data persists

**Solution**:
```bash
# Clear cache
curl -X DELETE http://localhost:5000/clear_market_data_cache

# Or manually
rm python_api/market_data_cache.db
```

### Volatility Issues

**Problem**: Volatility seems incorrect

**Cause**: Insufficient historical data or recent corporate action

**Solution**:
```python
# Manually specify volatility
market_data = {
    'AAPL': {
        'spot': 180.0,
        'rate': 0.05,
        'vol': 0.30  # Manual override
    }
}
```

### Network Issues

**Problem**: Frequent timeouts

**Solutions**:
- Increase timeout: `requests.post(..., timeout=60)`
- Use cached data
- Implement retry logic
- Check Yahoo Finance status

## See Also

- [API Reference](API.md) - Complete endpoint documentation
- [Development Guide](DEVELOPMENT.md) - Testing and contributing
- [Installation Guide](INSTALLATION.md) - Setup instructions