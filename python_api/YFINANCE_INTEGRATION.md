# YFinance Integration Guide

## Table of Contents
- [Quick Start](#quick-start)
- [Installation](#installation)
- [Architecture](#architecture)
- [API Reference](#api-reference)
- [Cache Management](#cache-management)
- [Error Handling](#error-handling)
- [Testing](#testing)
- [Performance](#performance)
- [Best Practices](#best-practices)

---

## Quick Start

### 1. Install Dependencies

```bash
cd python_api
pip install -r requirements.txt
```

### 2. Start the API Server

```bash
python app.py
```

### 3. Fetch Market Data

```bash
curl -X POST http://127.0.0.1:5000/update_market_data \
  -H "Content-Type: application/json" \
  -d '{"tickers": ["AAPL", "GOOGL", "MSFT"]}'
```

### 4. Use in Portfolio Risk Calculation

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

---

## Installation

### Prerequisites

- Python 3.11+
- pip package manager
- Internet connection for YFinance API

### Step-by-Step Installation

1. **Navigate to Python API directory:**
   ```bash
   cd python_api
   ```

2. **Create virtual environment (recommended):**
   ```bash
   python -m venv venv
   source venv/bin/activate  # Linux/Mac
   # venv\Scripts\activate   # Windows
   ```

3. **Install dependencies:**
   ```bash
   pip install -r requirements.txt
   ```

4. **Verify installation:**
   ```bash
   python -c "import yfinance; print('YFinance installed successfully')"
   ```

5. **Build Python bindings (if not done):**
   ```bash
   python setup.py build_ext --inplace
   ```

6. **Run tests:**
   ```bash
   pytest test_market_data.py -v
   ```

---

## Architecture

### Component Overview

```
┌─────────────────────────────────────────────────────────┐
│                    Flask API (app.py)                    │
│             /update_market_data endpoint                 │
└───────────────────────┬─────────────────────────────────┘
                        │
                        ▼
┌─────────────────────────────────────────────────────────┐
│         MarketDataFetcher (market_data_fetcher.py)      │
│          - fetch_single()                                │
│          - fetch_multiple()                              │
│          - _calculate_volatility()                       │
└───────────────────────┬─────────────────────────────────┘
                        │
          ┌─────────────┴─────────────┐
          ▼                           ▼
┌──────────────────┐         ┌──────────────────┐
│  YFinance API    │         │ MarketDataCache  │
│  (Yahoo Finance) │         │   (SQLite DB)    │
└──────────────────┘         └──────────────────┘
```

### Data Flow

1. **Request arrives** at `/update_market_data` endpoint
2. **Validation** checks ticker symbols and parameters
3. **Cache check** looks for recent data (< 24 hours)
4. **Fetch from YFinance** if cache miss or force_refresh
5. **Calculate volatility** from historical price data
6. **Store in cache** for future use
7. **Return response** with updated data

### File Structure

```
python_api/
├── app.py                      # Flask API with new endpoints
├── market_data_fetcher.py      # Core fetching logic
├── test_market_data.py         # Test suite
├── example_yfinance_usage.py   # Usage examples
├── market_data_cache.db        # SQLite cache (generated)
├── requirements.txt            # Dependencies
└── .env.example                # Configuration template
```

---

## API Reference

### Endpoint: POST /update_market_data

Fetch live market data from Yahoo Finance.

**Request:**
```json
{
  "tickers": ["AAPL", "GOOGL", "MSFT"],
  "force_refresh": false
}
```

**Parameters:**
| Name | Type | Required | Default | Description |
|------|------|----------|---------|-------------|
| `tickers` | array | Yes | - | List of ticker symbols (max 50) |
| `force_refresh` | boolean | No | false | Bypass cache and fetch fresh data |

**Response (200 OK):**
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
      "last_updated": "2025-10-25T14:30:00",
      "source": "yfinance"
    }
  },
  "failed": [],
  "summary": {
    "total_requested": 1,
    "successful": 1,
    "failed": 0
  },
  "timestamp": "2025-10-25T14:30:00"
}
```

**Response (207 Multi-Status):**
Returned when some tickers succeed and others fail.

**Error Responses:**
| Code | Description |
|------|-------------|
| 400 | Invalid request (missing tickers, invalid format) |
| 500 | Internal server error |

---

### Endpoint: GET /get_cached_market_data

Retrieve cached market data without fetching new data.

**Query Parameters:**
| Name | Type | Required | Description |
|------|------|----------|-------------|
| `asset_id` | string | No | Specific asset (omit for all) |

**Response:**
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

---

### Endpoint: DELETE /clear_market_data_cache

Clear all cached market data.

**Response:**
```json
{
  "success": true,
  "message": "Market data cache cleared",
  "timestamp": "2025-10-25T14:35:00"
}
```

---

## Cache Management

### Cache Structure

The cache is implemented using SQLite with the following schema:

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

**Cache Hit:**
- Data exists in cache
- Data age < 24 hours (configurable)
- Returns cached data immediately

**Cache Miss:**
- Data not in cache, or
- Data age > 24 hours
- Fetches from YFinance
- Updates cache

**Force Refresh:**
- `force_refresh: true` in request
- Always fetches from YFinance
- Overwrites cache

### Cache Expiration

Default expiration: **24 hours**

Configure via environment variable:
```bash
MARKET_DATA_CACHE_HOURS=12  # 12-hour expiration
```

### Cache Maintenance

```python
from market_data_fetcher import get_market_data_fetcher

fetcher = get_market_data_fetcher()

# View all cached data
all_data = fetcher.cache.get_all()
print(f"Cached assets: {len(all_data)}")

# Delete specific asset
fetcher.cache.delete('AAPL')

# Clear entire cache
fetcher.cache.clear()
```

---

## Error Handling

### Common Errors

#### 1. Invalid Ticker Symbol

**Error:**
```json
{
  "failed": [
    {
      "ticker": "INVALID",
      "error": "Failed to fetch data for INVALID: No price data available"
    }
  ]
}
```

**Solution:** Verify ticker symbol on Yahoo Finance

#### 2. Network Timeout

**Error:**
```json
{
  "error": "Failed to fetch data for AAPL: Connection timeout"
}
```

**Solution:** Check internet connection, retry request

#### 3. Rate Limiting

**Error:**
```json
{
  "error": "Too many requests"
}
```

**Solution:** Reduce request frequency, use caching

#### 4. Insufficient Historical Data

**Behavior:** Returns default volatility (0.25)

**Reason:** Recent IPO or limited trading history

**Solution:** Manually specify volatility in risk calculation

### Error Handling Best Practices

```python
import requests

def safe_fetch_market_data(tickers, max_retries=3):
    """Fetch market data with retry logic"""
    for attempt in range(max_retries):
        try:
            response = requests.post(
                'http://127.0.0.1:5000/update_market_data',
                json={'tickers': tickers},
                timeout=30
            )
            
            if response.status_code in [200, 207]:
                return response.json()
            
            if response.status_code >= 500:
                # Server error, retry
                continue
            else:
                # Client error, don't retry
                return {'error': response.json()}
                
        except requests.exceptions.Timeout:
            if attempt < max_retries - 1:
                continue
            return {'error': 'Timeout after retries'}
        except requests.exceptions.ConnectionError:
            return {'error': 'Cannot connect to API'}
    
    return {'error': 'Max retries exceeded'}
```

---

## Testing

### Running Tests

```bash
# All tests
pytest test_market_data.py -v

# Specific test class
pytest test_market_data.py::TestMarketDataCache -v

# With coverage
pytest test_market_data.py --cov=market_data_fetcher

# Skip network tests
pytest test_market_data.py -m "not network"
```

### Test Coverage

Current test coverage includes:

- ✅ Cache initialization and setup
- ✅ Cache CRUD operations
- ✅ Cache expiration logic
- ✅ Data fetching and validation
- ✅ Error handling
- ✅ API endpoint functionality
- ✅ Performance benchmarks

### Writing Custom Tests

```python
import pytest
from market_data_fetcher import MarketDataFetcher, MarketDataCache

def test_custom_cache_expiration():
    """Test custom cache expiration"""
    cache = MarketDataCache(':memory:')  # In-memory for testing
    cache.set('TEST', 100.0, 0.25, 0.045)
    
    # Should be available with 1 hour expiration
    data = cache.get('TEST', max_age_hours=1)
    assert data is not None
    
    # Should be expired with 0 hour expiration
    data = cache.get('TEST', max_age_hours=0)
    assert data is None
```

---

## Performance

### Benchmarks

| Operation | Latency | Notes |
|-----------|---------|-------|
| Cache read | < 1 ms | SQLite query |
| Cache write | < 5 ms | SQLite insert |
| Single ticker fetch | 1-3 s | Network dependent |
| 10 tickers fetch | 5-10 s | Sequential fetches |
| Volatility calculation | 10-50 ms | 252 days of data |

### Optimization Strategies

#### 1. Batch Fetching

```python
# Good: Fetch multiple tickers at once
response = requests.post('/update_market_data',
    json={'tickers': ['AAPL', 'GOOGL', 'MSFT']})

# Bad: Individual requests
for ticker in ['AAPL', 'GOOGL', 'MSFT']:
    response = requests.post('/update_market_data',
        json={'tickers': [ticker]})
```

#### 2. Cache Warming

```python
# Pre-populate cache before market open
import schedule
import time

def warm_cache():
    """Update cache with portfolio assets"""
    portfolio_tickers = ['AAPL', 'GOOGL', 'MSFT', 'AMZN']
    requests.post('/update_market_data',
        json={'tickers': portfolio_tickers, 'force_refresh': True})

# Schedule for 9:00 AM daily
schedule.every().day.at("09:00").do(warm_cache)

while True:
    schedule.run_pending()
    time.sleep(60)
```

#### 3. Conditional Refresh

```python
def smart_fetch(ticker, max_age_hours=6):
    """Only fetch if cache is stale"""
    # Check cache age first
    response = requests.get(f'/get_cached_market_data?asset_id={ticker}')
    
    if response.status_code == 200:
        data = response.json()[ticker]
        # Parse last_updated and check age
        # Only fetch if stale
        pass
    
    # Fetch if needed
    requests.post('/update_market_data',
        json={'tickers': [ticker]})
```

---

## Best Practices

### 1. Cache Management

```python
# ✅ Good: Check cache before fetching
cached = requests.get('/get_cached_market_data?asset_id=AAPL')
if cached.status_code == 404:
    requests.post('/update_market_data', json={'tickers': ['AAPL']})

# ❌ Bad: Always fetch without checking cache
requests.post('/update_market_data',
    json={'tickers': ['AAPL'], 'force_refresh': True})
```

### 2. Error Handling

```python
# ✅ Good: Handle partial failures
response = requests.post('/update_market_data',
    json={'tickers': ['AAPL', 'INVALID', 'GOOGL']})

data = response.json()
if data['failed']:
    # Use cached data or manual input for failed tickers
    for failure in data['failed']:
        print(f"Using fallback for {failure['ticker']}")

# ❌ Bad: Assume all succeed
data = response.json()
all_data = data['updated']  # May be incomplete!
```

### 3. Rate Limiting

```python
# ✅ Good: Respect rate limits
import time

def fetch_large_universe(tickers, batch_size=50):
    """Fetch many tickers in batches"""
    for i in range(0, len(tickers), batch_size):
        batch = tickers[i:i+batch_size]
        requests.post('/update_market_data', json={'tickers': batch})
        if i + batch_size < len(tickers):
            time.sleep(1)  # Rate limit protection

# ❌ Bad: Request all at once
requests.post('/update_market_data',
    json={'tickers': list_of_1000_tickers})
```

### 4. Data Validation

```python
# ✅ Good: Validate fetched data
data = response.json()['updated']['AAPL']
if data['spot'] <= 0:
    raise ValueError("Invalid spot price")
if not (0.01 <= data['vol'] <= 2.0):
    print("Warning: Unusual volatility")

# ❌ Bad: Use data without validation
spot = data['spot']
vol = data['vol']
```

### 5. Fallback Strategies

```python
# ✅ Good: Multiple fallback options
def get_market_data(ticker):
    # Try cache first
    cached = get_cached_data(ticker)
    if cached:
        return cached
    
    # Try YFinance
    try:
        fetched = fetch_from_yfinance(ticker)
        return fetched
    except:
        pass
    
    # Use manual/default values
    return {
        'spot': get_last_known_price(ticker),
        'vol': 0.25,
        'rate': 0.045
    }
```

---

## Troubleshooting

### Issue: YFinance returns no data

**Symptoms:**
```json
{
  "failed": [{"ticker": "AAPL", "error": "No price data available"}]
}
```

**Possible Causes:**
1. Ticker is delisted
2. Market is closed and no historical data
3. Yahoo Finance API is down
4. Network connectivity issues

**Solutions:**
1. Verify ticker on finance.yahoo.com
2. Use `force_refresh: true`
3. Check Yahoo Finance status
4. Test network connection

### Issue: Volatility calculation fails

**Symptoms:** Default volatility (0.25) returned

**Possible Causes:**
1. Recent IPO (< 1 year history)
2. Illiquid stock with sparse data
3. Corporate action (split, merger)

**Solutions:**
1. Manually specify volatility
2. Use comparable stock's volatility
3. Calculate from options implied vol

### Issue: Cache not updating

**Symptoms:** Old data returned despite `force_refresh`

**Possible Causes:**
1. Database lock
2. Permission issues
3. Disk space full

**Solutions:**
```bash
# Check database
sqlite3 market_data_cache.db ".tables"

# Clear and rebuild
rm market_data_cache.db
python -c "from market_data_fetcher import MarketDataCache; MarketDataCache()"

# Check permissions
ls -la market_data_cache.db
chmod 644 market_data_cache.db
```

---

## Support

For issues or questions:
- GitHub Issues: https://github.com/Quant-Enthusiasts/Quant-Enthusiasts-Risk-Engine/issues
- Discord: https://discord.com/invite/z3S9Fguzw3
- Email: support@quantenthusiasts.org

---

## License

This integration is part of the Quant Enthusiasts Risk Engine and is licensed under the MIT License.
