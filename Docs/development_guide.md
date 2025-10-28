# Development Guide

Guide for contributing to the Quant Enthusiasts Risk Engine.

## Development Setup

### Prerequisites

Follow [INSTALLATION.md](INSTALLATION.md) for basic setup, then install development dependencies:

```bash
cd python_api
pip install -r requirements-dev.txt
```

**Development dependencies:**
- pytest - Testing framework
- pytest-cov - Code coverage
- pytest-mock - Mocking utilities
- black - Code formatter
- flake8 - Linter
- mypy - Type checker

### IDE Setup

**VS Code:**

Install extensions:
- C/C++ (Microsoft)
- Python (Microsoft)
- CMake Tools
- GitLens

Recommended `settings.json`:
```json
{
  "python.linting.enabled": true,
  "python.linting.flake8Enabled": true,
  "python.formatting.provider": "black",
  "editor.formatOnSave": true,
  "[cpp]": {
    "editor.defaultFormatter": "ms-vscode.cpptools"
  }
}
```

**CLion / PyCharm:**

1. Open project root as CMake project
2. Set Python interpreter to virtual environment
3. Enable ClangFormat for C++ formatting

## Project Structure

```
Quant-Enthusiasts-Risk-Engine/
├── cpp_engine/
│   ├── apps/                    # Application entry points
│   │   └── main.cpp            # C++ demo application
│   ├── libraries/
│   │   ├── qe_risk_engine/     # Core risk engine
│   │   │   ├── src/            # Implementation files
│   │   │   └── includes/       # Public headers
│   │   └── python_interface/   # pybind11 bindings
│   │       └── src/
│   ├── tests/                   # C++ unit tests
│   ├── CMakeLists.txt          # Build configuration
│   └── build.sh                # Build automation script
├── python_api/
│   ├── app.py                  # Flask application
│   ├── market_data_fetcher.py  # YFinance integration
│   ├── setup.py                # Python bindings build
│   ├── requirements.txt        # Production dependencies
│   ├── requirements-dev.txt    # Development dependencies
│   └── test_market_data.py     # API tests
├── js_dashboard/
│   └── index.html              # Web interface
├── docs/                        # Documentation
└── .github/workflows/          # CI/CD pipelines
```

## Testing

### C++ Tests

The C++ codebase uses Google Test for unit testing.

**Run all tests:**
```bash
cd cpp_engine/build
ctest --output-on-failure
```

**Run specific test suite:**
```bash
./tests/test_blackscholes
./tests/test_portfolio
./tests/test_risk_engine
```

**Run from project root:**
```bash
./run_tests.sh
```

**Test coverage:**
- BlackScholes pricing and Greeks
- Binomial tree (American/European)
- Jump diffusion model
- Portfolio aggregation
- Risk engine VaR calculations

### Python Tests

**Run all Python tests:**
```bash
cd python_api
pytest test_market_data.py -v
```

**Run with coverage:**
```bash
pytest test_market_data.py --cov=market_data_fetcher --cov-report=html
```

**Run specific test class:**
```bash
pytest test_market_data.py::TestMarketDataCache -v
```

**Skip network tests:**
```bash
pytest test_market_data.py -m "not network"
```

### Integration Tests

Full stack end-to-end tests are part of the CI pipeline. Run locally:

```bash
# Start API server
cd python_api
python app.py &
API_PID=$!

# Wait for server
sleep 2

# Run integration tests
curl http://127.0.0.1:5000/health

curl -X POST http://127.0.0.1:5000/calculate_risk \
  -H "Content-Type: application/json" \
  -d '{"portfolio": [...], "market_data": {...}}'

# Cleanup
kill $API_PID
```

### Adding New Tests

**C++ Test Example:**
```cpp
#include <gtest/gtest.h>
#include "BlackScholes.h"

TEST(BlackScholesTest, CallPriceInTheMoney) {
    double S = 105.0;  // Spot
    double K = 100.0;  // Strike
    double T = 1.0;    // Expiry
    double r = 0.05;   // Rate
    double sigma = 0.25;  // Vol
    
    double price = BlackScholes::call_price(S, K, T, r, sigma);
    EXPECT_GT(price, 5.0);  // In-the-money, should be > intrinsic
    EXPECT_LT(price, S);     // Should be less than spot
}
```

**Python Test Example:**
```python
import pytest
from market_data_fetcher import MarketDataFetcher

def test_fetch_valid_ticker():
    """Test fetching data for valid ticker"""
    fetcher = MarketDataFetcher()
    data = fetcher.fetch_single('AAPL')
    
    assert data['asset_id'] == 'AAPL'
    assert data['spot'] > 0
    assert 0 < data['vol'] < 2.0
```

## Code Style

### C++

Follow Google C++ Style Guide with modifications:

**Naming conventions:**
- Classes: `PascalCase` (e.g., `RiskEngine`)
- Functions: `snake_case` (e.g., `calculate_risk()`)
- Variables: `snake_case` (e.g., `spot_price`)
- Constants: `UPPER_SNAKE_CASE` (e.g., `MAX_SIMULATIONS`)

**Formatting:**
```bash
# Format all C++ files
find cpp_engine -name "*.cpp" -o -name "*.h" | xargs clang-format -i
```

**Example:**
```cpp
class RiskEngine {
public:
    explicit RiskEngine(int simulations = 100000);
    
    double calculate_var(const Portfolio& portfolio,
                        const MarketData& market_data,
                        double confidence_level = 0.95);
                        
private:
    int num_simulations_;
    std::mt19937 rng_;
};
```

### Python

Follow PEP 8 with Black formatting:

**Format Python files:**
```bash
black python_api/
```

**Check style:**
```bash
flake8 python_api/ --max-line-length=100
```

**Type hints:**
```python
def fetch_market_data(ticker: str, force_refresh: bool = False) -> Dict[str, Any]:
    """
    Fetch market data from YFinance
    
    Args:
        ticker: Stock ticker symbol
        force_refresh: Bypass cache if True
        
    Returns:
        Dictionary with market data fields
        
    Raises:
        ValueError: If ticker is invalid
    """
    pass
```

### Documentation

**C++ Comments:**
```cpp
/**
 * Calculate European call option price using Black-Scholes formula
 * 
 * @param S Current spot price
 * @param K Strike price
 * @param T Time to expiry (years)
 * @param r Risk-free rate
 * @param sigma Volatility
 * @return Option price
 */
double call_price(double S, double K, double T, double r, double sigma);
```

**Python Docstrings:**
```python
def calculate_volatility(prices: pd.Series) -> float:
    """
    Calculate annualized historical volatility from price series
    
    Uses log returns and assumes 252 trading days per year.
    
    Args:
        prices: Time series of historical prices
        
    Returns:
        Annualized volatility as decimal (e.g., 0.25 for 25%)
        
    Example:
        >>> prices = pd.Series([100, 102, 101, 103])
        >>> vol = calculate_volatility(prices)
        >>> print(f"Volatility: {vol*100:.2f}%")
    """
    pass
```

## CI/CD Pipeline

### GitHub Actions Workflow

The project uses GitHub Actions for continuous integration. Pipeline runs on:
- Every push to `main`
- All pull requests
- Manual workflow dispatch

**Pipeline stages:**

1. **C++ Build Matrix** (8 combinations)
   - Ubuntu + GCC/Clang x Debug/Release
   - macOS + Clang x Debug/Release

2. **Python API Tests** (6 combinations)
   - Python 3.9, 3.10, 3.11 x Ubuntu/macOS

3. **Frontend Validation**
   - HTML validation
   - CDN accessibility checks
   - JavaScript syntax validation

4. **Docker Build & Test**
   - Full container build
   - Bindings verification
   - Flask startup test

5. **Integration Tests**
   - Full stack E2E testing
   - Real Flask API validation

6. **Code Quality Checks**
   - Line ending validation
   - File permission checks
   - TODO scanning

7. **Performance Benchmarks** (main branch only)
   - Timed risk engine execution

### Running CI Locally

**Lint checks:**
```bash
# C++
cppcheck cpp_engine/src/

# Python
flake8 python_api/
mypy python_api/
```

**Build checks:**
```bash
# Clean build
cd cpp_engine
rm -rf build
mkdir build && cd build
cmake .. -DCMAKE_BUILD_TYPE=Release
cmake --build .
```

**Test suite:**
```bash
# C++ tests
ctest --output-on-failure

# Python tests
pytest python_api/ -v --cov
```

## Contributing

### Workflow

1. **Fork the repository**
   ```bash
   gh repo fork Quant-Enthusiasts/Quant-Enthusiasts-Risk-Engine
   ```

2. **Create a feature branch**
   ```bash
   git checkout -b feature/amazing-feature
   ```

3. **Make your changes**
   - Write code
   - Add tests
   - Update documentation

4. **Run tests locally**
   ```bash
   ./run_tests.sh
   pytest python_api/test_market_data.py -v
   ```

5. **Commit with clear messages**
   ```bash
   git commit -m "feat: Add support for exotic options"
   ```

6. **Push and create PR**
   ```bash
   git push origin feature/amazing-feature
   gh pr create
   ```

### Commit Message Convention

Use conventional commits:

- `feat:` New feature
- `fix:` Bug fix
- `docs:` Documentation changes
- `test:` Test additions/changes
- `refactor:` Code refactoring
- `perf:` Performance improvements
- `ci:` CI/CD changes

**Examples:**
```
feat: Add Greeks calculation for American options
fix: Correct volatility calculation for sparse data
docs: Update API reference with new endpoints
test: Add integration tests for market data fetcher
```

### Pull Request Checklist

Before submitting:

- [ ] Code follows project style guidelines
- [ ] All tests pass locally
- [ ] New tests added for new features
- [ ] Documentation updated
- [ ] Commit messages follow convention
- [ ] No merge conflicts with main
- [ ] PR description explains changes clearly

### Review Process

1. Automated CI checks must pass
2. At least one maintainer review required
3. Address all review comments
4. Squash commits if requested
5. Maintainer will merge when approved

## Performance Profiling

### C++ Profiling

**Using gprof:**
```bash
cmake .. -DCMAKE_CXX_FLAGS="-pg"
cmake --build .
./risk-engine
gprof ./risk-engine gmon.out > analysis.txt
```

**Using Valgrind:**
```bash
valgrind --tool=callgrind ./risk-engine
kcachegrind callgrind.out.*
```

### Python Profiling

**Using cProfile:**
```python
import cProfile
import pstats

profiler = cProfile.Profile()
profiler.enable()

# Your code here
result = engine.calculate_portfolio_risk(portfolio, market_data)

profiler.disable()
stats = pstats.Stats(profiler)
stats.sort_stats('cumulative')
stats.print_stats(20)
```

**Using line_profiler:**
```bash
pip install line_profiler
kernprof -l -v app.py
```

## Debugging

### C++ Debugging

**GDB:**
```bash
cmake .. -DCMAKE_BUILD_TYPE=Debug
cmake --build .
gdb ./risk-engine
(gdb) break BlackScholes::call_price
(gdb) run
(gdb) print spot_price
```

**LLDB (macOS):**
```bash
lldb ./risk-engine
(lldb) breakpoint set --name call_price
(lldb) run
(lldb) frame variable
```

### Python Debugging

**pdb:**
```python
import pdb; pdb.set_trace()
```

**VS Code Debugger:**

`.vscode/launch.json`:
```json
{
  "version": "0.2.0",
  "configurations": [
    {
      "name": "Python: Flask",
      "type": "python",
      "request": "launch",
      "module": "flask",
      "env": {
        "FLASK_APP": "app.py",
        "FLASK_ENV": "development"
      },
      "args": ["run", "--no-debugger"],
      "jinja": true
    }
  ]
}
```

## Release Process

1. Update version numbers:
   - `python_api/setup.py`
   - `python_api/app.py` (health endpoint)
   - `README.md` badges

2. Update CHANGELOG.md

3. Create release branch:
   ```bash
   git checkout -b release/v3.1.0
   ```

4. Run full test suite

5. Create GitHub release with tag

6. Build and push Docker image:
   ```bash
   docker build -t quantenthusiasts/risk-engine:v3.1.0 .
   docker push quantenthusiasts/risk-engine:v3.1.0
   ```

## Getting Help

- **Discord**: [Join developer channel](https://discord.com/invite/z3S9Fguzw3)
- **GitHub Issues**: [Report bugs or ask questions](https://github.com/Quant-Enthusiasts/Quant-Enthusiasts-Risk-Engine/issues)
- **Email**: dev@quantenthusiasts.org