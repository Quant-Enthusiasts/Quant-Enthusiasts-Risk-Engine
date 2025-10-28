# Frequently Asked Questions

Common questions about the Quant Enthusiasts Risk Engine.

## General Questions

### What is the Quant Enthusiasts Risk Engine?

A high-performance quantitative finance platform for portfolio risk management and options pricing. It combines a C++17 computational core with Python API services and a web-based dashboard.

### Who should use this?

- Quantitative analysts and traders
- Portfolio managers
- Financial engineers
- Students learning quantitative finance
- Researchers in financial mathematics
- Developers building fintech applications

### Is it production-ready?

The engine is suitable for research, education, and internal tools. For production trading:
- Add authentication and authorization
- Implement rate limiting
- Add comprehensive logging
- Set up monitoring and alerting
- Review and test thoroughly

See [DEPLOYMENT.md](DEPLOYMENT.md) for production considerations.

### What license is it under?

MIT License. You can use it freely for commercial and non-commercial purposes. See [LICENSE](LICENSE) for details.

### How do I cite this project?

```bibtex
@software{quant_enthusiasts_risk_engine,
  title = {Quant Enthusiasts Risk Engine},
  author = {Quant Enthusiasts},
  year = {2025},
  url = {https://github.com/Quant-Enthusiasts/Quant-Enthusiasts-Risk-Engine}
}
```

## Technical Questions

### What programming languages are used?

- **C++17**: Core computational engine
- **Python 3.11+**: API layer and bindings
- **JavaScript**: Web dashboard
- **SQL**: Market data caching

### Why C++ for the core?

C++ provides:
- 10-100x performance over pure Python
- Low memory overhead for Monte Carlo simulations
- Easy integration via pybind11
- Industry-standard for quantitative finance

### Can I use only the C++ engine without Python?

Yes. The C++ engine is standalone:

```bash
cd cpp_engine/build
./bin/risk-engine  # Standalone demo
```

You can also link the library directly in your C++ projects.

### Can I use only the Python API without building C++?

No. The Python API requires the C++ engine for calculations. However, pre-built wheels may be available in the future.

### What platforms are supported?

- **Linux**: Ubuntu 20.04+, Debian 10+, RHEL 8+
- **macOS**: 11+ (Big Sur and later)
- **Windows**: 10/11 with Visual Studio 2019+

See [INSTALLATION.md](INSTALLATION.md) for platform-specific instructions.

## Features and Capabilities

### What option types are supported?

**Option Styles**:
- European options (exercise at expiry only)
- American options (exercise any time)

**Option Types**:
- Call options
- Put options

**Coming Soon**:
- Asian options
- Barrier options
- Binary options

### What pricing models are available?

1. **Black-Scholes**: Analytical solution for European options
2. **Binomial Tree**: Numerical method for European and American options
3. **Merton Jump Diffusion**: Incorporates discontinuous price jumps

See [API.md](API.md) for usage examples.

### Can I calculate implied volatility?

Not directly in the current version. Workarounds:
- Calculate manually using Newton-Raphson
- Use cached volatility from historical data
- Fetch from external source

Planned for future releases.

### What risk metrics are calculated?

**Greeks**:
- Delta: Price sensitivity to spot
- Gamma: Delta sensitivity to spot
- Vega: Price sensitivity to volatility
- Theta: Price sensitivity to time
- Rho: Price sensitivity to interest rate

**Risk Measures**:
- Value at Risk (VaR) at 95% and 99% confidence
- Expected Shortfall (CVaR) at 95% and 99%
- Portfolio present value (PV)
- Net position by asset

### How accurate are the calculations?

**Black-Scholes**: Exact analytical solution

**Binomial Tree**: Converges to Black-Scholes as steps increase
- 100 steps: ±0.1% error
- 1000 steps: ±0.01% error

**Monte Carlo VaR**: Statistical accuracy improves with simulations
- 10K simulations: ~1% standard error
- 100K simulations: ~0.3% standard error
- 1M simulations: ~0.1% standard error

Validated against known benchmarks in test suite.

### What is the maximum portfolio size?

Tested configurations:
- 1,000+ instruments: < 5 seconds
- 10,000+ instruments: < 30 seconds

Limits depend on:
- Available RAM
- VaR simulation count
- Server resources

### How fast are the calculations?

Typical latencies:
- Single option pricing: < 1 microsecond
- Portfolio (100 options): < 1 millisecond
- VaR (100K simulations): < 1 second
- Market data fetch: 1-3 seconds per ticker

See [ARCHITECTURE.md](ARCHITECTURE.md) for performance details.

## Market Data Questions

### Where does market data come from?

Yahoo Finance via the YFinance Python library. Data includes:
- Current spot prices
- Historical prices (for volatility calculation)
- Dividend yields
- 10-year Treasury rate (risk-free rate)

### Is the market data real-time?

Near real-time during market hours (15-20 minute delay typical). Outside market hours, uses last closing prices.

### Can I use my own market data?

Yes. Provide data directly in API requests:

```json
{
  "market_data": {
    "AAPL": {
      "spot": 175.0,
      "vol": 0.28,
      "rate": 0.045,
      "dividend": 0.005
    }
  }
}
```

### How is volatility calculated?

Historical volatility from 252 trading days (1 year):
1. Fetch daily prices
2. Calculate log returns: `log(P_t / P_{t-1})`
3. Compute standard deviation
4. Annualize: `σ_annual = σ_daily * sqrt(252)`

Default: 25% if insufficient data.

### Can I use implied volatility?

Not automatically. You can:
- Calculate from option prices
- Fetch from data provider
- Specify manually in request

### Does it support international markets?

Yes, any ticker available on Yahoo Finance:
- US: `AAPL`
- UK: `BARC.L`
- Germany: `SAP.DE`
- Japan: `7203.T`

Verify ticker symbols on [finance.yahoo.com](https://finance.yahoo.com).

### How long is data cached?

Default: 24 hours

Configure via environment variable:
```bash
MARKET_DATA_CACHE_HOURS=12  # 12-hour cache
```

### What if market data fetch fails?

Graceful degradation:
1. Try cache first
2. Attempt live fetch
3. Return error if both fail

You can always provide data manually as fallback.

## Usage Questions

### How do I price a single option?

```bash
curl -X POST http://localhost:5000/price_option \
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

See [QUICKSTART.md](QUICKSTART.md) for more examples.

### How do I calculate portfolio risk?

```bash
curl -X POST http://localhost:5000/calculate_risk \
  -H "Content-Type: application/json" \
  -d '{
    "portfolio": [
      {"type": "call", "strike": 100, "expiry": 1.0, "asset_id": "AAPL", "quantity": 100, "style": "european"}
    ],
    "market_data": {}
  }'
```

### Do I need to provide market data every time?

No. If `market_data` is empty or incomplete, the engine:
1. Checks cache for recent data
2. Fetches from Yahoo Finance if needed
3. Uses cached data automatically

### Can I run calculations offline?

Yes, if you:
1. Pre-populate cache with market data
2. Provide market data in requests
3. Disable auto-fetch (future feature)

### How do I compare different pricing models?

Send multiple requests with different `pricing_model` parameters:

```bash
# Black-Scholes
curl ... -d '{"pricing_model": "blackscholes", ...}'

# Binomial
curl ... -d '{"pricing_model": "binomial", "binomial_steps": 1000, ...}'

# Jump Diffusion
curl ... -d '{"pricing_model": "jumpdiffusion", "jump_parameters": {...}, ...}'
```

### Can I save and load portfolios?

Not built-in currently. You can:
- Save portfolio JSON to file
- Use external database
- Implement custom storage

Planned feature for future releases.

### How do I debug calculation errors?

1. Run Flask in debug mode:
```bash
python -m flask --app app run --debug
```

2. Check input validation:
```python
# Ensure all required fields present
# Verify data types (float vs string)
# Check value ranges (spot > 0, etc.)
```

3. Test with minimal portfolio:
```json
{
  "portfolio": [
    {"type": "call", "strike": 100, "expiry": 1.0, "asset_id": "AAPL", "quantity": 1, "style": "european"}
  ],
  "market_data": {"AAPL": {"spot": 105, "rate": 0.05, "vol": 0.25}}
}
```

## Deployment Questions

### Can I deploy this in production?

Yes, with proper security measures:
- Add authentication (API keys, OAuth2)
- Implement rate limiting
- Use HTTPS (TLS/SSL certificates)
- Set up monitoring and logging
- Run behind reverse proxy (Nginx)

See [DEPLOYMENT.md](DEPLOYMENT.md) for complete guide.

### Does it support Docker?

Yes. Dockerfile included:

```bash
docker build -t risk-engine .
docker run -p 5000:5000 risk-engine
```

Also supports Docker Compose for multi-container deployment.

### Can it run on AWS/GCP/Azure?

Yes. Deployment guides for:
- AWS ECS/Fargate
- Google Cloud Run
- Azure Container Instances
- DigitalOcean App Platform

See [DEPLOYMENT.md](DEPLOYMENT.md) for configuration examples.

### How do I scale horizontally?

1. Deploy multiple instances
2. Use load balancer (HAProxy, Nginx, ALB)
3. Share cache via Redis
4. Use distributed task queue (Celery)

### What about database requirements?

Built-in SQLite for cache (single-user).

For production:
- PostgreSQL for persistent storage
- Redis for distributed cache
- TimescaleDB for time-series data

### How much does it cost to run?

**Free**: Self-hosted on your infrastructure

**Cloud costs** (approximate):
- AWS: $50-200/month (t3.medium + ALB)
- GCP: $40-150/month (Cloud Run)
- Azure: $60-180/month (Container Instances)
- DigitalOcean: $20-100/month (App Platform)

Depends on traffic, compute size, and data transfer.

## Development Questions

### How do I contribute?

1. Fork repository
2. Create feature branch
3. Make changes with tests
4. Submit pull request

See [CONTRIBUTING.md](CONTRIBUTING.md) for detailed guide.

### What's the tech stack?

**Backend**:
- C++17 (core engine)
- Python 3.11+ (Flask API)
- SQLite (cache)
- pybind11 (bindings)

**Frontend**:
- HTML5
- JavaScript (ES6+)
- Tailwind CSS

**Tools**:
- CMake (build system)
- pytest (Python testing)
- Google Test (C++ testing)
- GitHub Actions (CI/CD)

### Where's the documentation?

- **README.md**: Overview and quick start
- **API.md**: Complete API reference
- **INSTALLATION.md**: Setup instructions
- **DEVELOPMENT.md**: Contributing guide
- **ARCHITECTURE.md**: System design
- **DEPLOYMENT.md**: Production deployment

### How do I report bugs?

1. Search existing issues
2. Create new issue with:
   - Clear description
   - Steps to reproduce
   - System information
   - Error messages

[Report bug on GitHub](https://github.com/Quant-Enthusiasts/Quant-Enthusiasts-Risk-Engine/issues)

### Can I request features?

Yes! Open a feature request issue:
- Describe the feature
- Explain use case
- Provide examples if possible

Popular requests may be prioritized.

### Is there a roadmap?

Current priorities:
- Implied volatility calculation
- More exotic options (Asian, Barrier)
- Real-time streaming via WebSocket
- Portfolio optimization
- Multiple data source support

See [GitHub Projects](https://github.com/Quant-Enthusiasts/Quant-Enthusiasts-Risk-Engine/projects) for detailed roadmap.

## Support Questions

### Where can I get help?

**Community**:
- [Discord](https://discord.com/invite/z3S9Fguzw3): Real-time chat
- GitHub Discussions: Async Q&A
- GitHub Issues: Bug reports

**Documentation**:
- Check [TROUBLESHOOTING.md](TROUBLESHOOTING.md) first
- Review relevant guides in docs/

**Email**:
- support@quantenthusiasts.org (general support)
- dev@quantenthusiasts.org (technical questions)

### Is there commercial support?

Not currently. Community support only via Discord and GitHub.

Interested in commercial support/consulting? Email: business@quantenthusiasts.org

### Can I hire someone to help?

Post in:
- Discord #jobs channel
- GitHub Discussions
- Freelance platforms (Upwork, Fiverr)

### What if I found a security issue?

**Do not** open public issue.

Email: security@quantenthusiasts.org

Include:
- Description of vulnerability
- Steps to reproduce
- Potential impact

We'll respond within 48 hours.

## Performance Questions

### Why are calculations slow?

Common causes:
- Debug build (use Release)
- Too many VaR simulations
- Large portfolio (batch processing)
- Network latency (market data fetch)

See [TROUBLESHOOTING.md](TROUBLESHOOTING.md#performance-issues).

### How can I improve performance?

1. Use Release build
2. Reduce VaR simulations
3. Use caching effectively
4. Batch portfolio calculations
5. Consider C++ direct integration

### Can I use multiple cores?

VaR Monte Carlo uses single core currently.

Future plans:
- OpenMP for parallel simulations
- GPU acceleration via CUDA
- Distributed computing support

### What's the memory footprint?

Typical usage:
- Base process: ~50 MB
- 100-option portfolio: ~100 MB
- 100K VaR simulations: ~150 MB
- 1M VaR simulations: ~500 MB

## Still Have Questions?

Join our [Discord community](https://discord.com/invite/z3S9Fguzw3) or open a [GitHub Discussion](https://github.com/Quant-Enthusiasts/Quant-Enthusiasts-Risk-Engine/discussions).