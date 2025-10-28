# Contributing Guide

Thank you for considering contributing to the Quant Enthusiasts Risk Engine! This guide will help you get started.

## Table of Contents

- [Code of Conduct](#code-of-conduct)
- [Getting Started](#getting-started)
- [Development Workflow](#development-workflow)
- [Contribution Types](#contribution-types)
- [Style Guidelines](#style-guidelines)
- [Testing Requirements](#testing-requirements)
- [Pull Request Process](#pull-request-process)
- [Community](#community)

## Code of Conduct

### Our Standards

We are committed to providing a welcoming and inclusive environment. We expect all contributors to:

- Use welcoming and inclusive language
- Be respectful of differing viewpoints and experiences
- Gracefully accept constructive criticism
- Focus on what is best for the community
- Show empathy towards other community members

### Unacceptable Behavior

- Harassment, discrimination, or offensive comments
- Trolling, insulting comments, or personal attacks
- Publishing others' private information without permission
- Other conduct inappropriate in a professional setting

Report unacceptable behavior to: conduct@quantenthusiasts.org

## Getting Started

### Prerequisites

Before contributing, ensure you have:

1. Read the [README.md](README.md)
2. Completed the [INSTALLATION.md](INSTALLATION.md) setup
3. Reviewed the [ARCHITECTURE.md](ARCHITECTURE.md) to understand system design
4. Joined our [Discord community](https://discord.com/invite/z3S9Fguzw3)

### First-Time Contributors

Good first issues are labeled with `good-first-issue` on GitHub:

```bash
# Find beginner-friendly issues
https://github.com/Quant-Enthusiasts/Quant-Enthusiasts-Risk-Engine/labels/good-first-issue
```

Popular starter tasks:
- Documentation improvements
- Test coverage additions
- Bug fixes with clear reproduction steps
- Code style improvements

## Development Workflow

### 1. Fork and Clone

```bash
# Fork repository on GitHub, then clone your fork
git clone https://github.com/YOUR_USERNAME/Quant-Enthusiasts-Risk-Engine.git
cd Quant-Enthusiasts-Risk-Engine

# Add upstream remote
git remote add upstream https://github.com/Quant-Enthusiasts/Quant-Enthusiasts-Risk-Engine.git
```

### 2. Create a Branch

Use descriptive branch names:

```bash
# Feature branch
git checkout -b feature/add-exotic-options

# Bug fix branch
git checkout -b fix/volatility-calculation

# Documentation branch
git checkout -b docs/api-examples
```

### 3. Make Changes

Follow the [Style Guidelines](#style-guidelines) section below.

### 4. Test Your Changes

```bash
# C++ tests
cd cpp_engine/build
ctest --output-on-failure

# Python tests
cd python_api
pytest test_market_data.py -v

# Run full test suite
./run_tests.sh
```

### 5. Commit

Use conventional commit messages:

```bash
git commit -m "feat: Add support for Asian options"
git commit -m "fix: Correct VaR calculation for negative positions"
git commit -m "docs: Update API reference with new endpoints"
git commit -m "test: Add integration tests for market data"
```

**Commit message types**:
- `feat:` New feature
- `fix:` Bug fix
- `docs:` Documentation changes
- `test:` Test additions or modifications
- `refactor:` Code refactoring without functionality change
- `perf:` Performance improvements
- `ci:` CI/CD changes
- `chore:` Maintenance tasks

### 6. Push and Create PR

```bash
git push origin feature/add-exotic-options
```

Then create a pull request on GitHub.

## Contribution Types

### Code Contributions

**New Features**:
- Discuss in GitHub issue before implementing
- Write tests for new functionality
- Update documentation
- Ensure backward compatibility

**Bug Fixes**:
- Reference the issue number in commit message
- Add regression test
- Document the fix in PR description

**Performance Improvements**:
- Include benchmarks showing improvement
- Explain trade-offs if any
- Test with various input sizes

### Documentation Contributions

**Types of documentation needed**:
- API endpoint examples
- Tutorial notebooks
- Architecture explanations
- Troubleshooting guides
- Translation to other languages

**Documentation standards**:
- Use clear, concise language
- Include code examples
- Test all code snippets
- Add diagrams where helpful

### Testing Contributions

**Needed tests**:
- Edge case coverage
- Error handling paths
- Integration tests
- Performance benchmarks
- Cross-platform tests

**Test standards**:
- Tests must be deterministic
- Use descriptive test names
- Include assertion messages
- Mock external dependencies

## Style Guidelines

### C++ Style

Follow Google C++ Style Guide with modifications:

**Naming**:
```cpp
class PortfolioManager;           // PascalCase for classes
void calculate_greeks();          // snake_case for functions
double spot_price_;               // snake_case with trailing underscore for members
const int MAX_SIMULATIONS = 1e6;  // UPPER_CASE for constants
```

**Formatting**:
- Indentation: 4 spaces (no tabs)
- Max line length: 100 characters
- Braces: Opening brace on same line
- Pointers/references: `Type* ptr` (asterisk with type)

**Example**:
```cpp
class RiskCalculator {
public:
    explicit RiskCalculator(int simulations = 100000);
    
    double calculate_var(const Portfolio& portfolio,
                        const MarketData& market_data,
                        double confidence = 0.95);
                        
private:
    int num_simulations_;
    std::mt19937 rng_;
    
    void initialize_random_generator();
};
```

**Format C++ code**:
```bash
clang-format -i cpp_engine/src/**/*.cpp
```

### Python Style

Follow PEP 8 with Black formatting:

**Naming**:
```python
class MarketDataFetcher:    # PascalCase for classes
def fetch_ticker(ticker):   # snake_case for functions
MAX_TICKERS = 50           # UPPER_CASE for constants
```

**Type hints**:
```python
def calculate_volatility(prices: pd.Series, 
                        window: int = 252) -> float:
    """
    Calculate annualized volatility
    
    Args:
        prices: Historical price series
        window: Rolling window size
        
    Returns:
        Annualized volatility
    """
    pass
```

**Format Python code**:
```bash
black python_api/
flake8 python_api/ --max-line-length=100
```

### Documentation Style

**Docstrings (Python)**:
```python
def calculate_risk(portfolio: List[Dict], 
                  market_data: Dict) -> Dict:
    """
    Calculate portfolio risk metrics
    
    Computes total PV, Greeks, and Value at Risk using Monte Carlo
    simulation. Market data is auto-fetched if not provided.
    
    Args:
        portfolio: List of option specifications
        market_data: Market data by asset (optional)
        
    Returns:
        Dictionary with risk metrics
        
    Raises:
        ValueError: If portfolio is empty or invalid
        
    Example:
        >>> result = calculate_risk([option1, option2], market_data)
        >>> print(f"Portfolio VaR: ${result['value_at_risk_95']:.2f}")
    """
    pass
```

**Comments (C++)**:
```cpp
/**
 * Calculate European call option price using Black-Scholes
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

### Markdown Style

- Use ATX-style headers (`#` not `===`)
- Code blocks must specify language
- Use tables for structured data
- Include examples for complex concepts

## Testing Requirements

### Required Tests

All contributions must include tests:

**C++ Unit Tests**:
```cpp
#include <gtest/gtest.h>
#include "Portfolio.h"

TEST(PortfolioTest, AddInstrument) {
    Portfolio portfolio;
    EuropeanOption call(OptionType::Call, 100.0, 1.0, "AAPL");
    
    portfolio.add_instrument(call, 10);
    
    EXPECT_EQ(portfolio.size(), 1);
    EXPECT_EQ(portfolio.get_total_quantity("AAPL"), 10);
}
```

**Python Unit Tests**:
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

### Test Coverage Goals

- New features: 100% coverage
- Bug fixes: Test reproducing the bug
- Refactoring: Maintain existing coverage

### Running Tests

```bash
# All C++ tests
cd cpp_engine/build
ctest --output-on-failure

# All Python tests
cd python_api
pytest -v --cov=. --cov-report=html

# Specific test file
pytest test_market_data.py::TestMarketDataCache -v
```

## Pull Request Process

### Before Submitting

**Checklist**:
- [ ] Code follows style guidelines
- [ ] All tests pass locally
- [ ] New tests added for new features
- [ ] Documentation updated
- [ ] Commit messages follow convention
- [ ] No merge conflicts with main branch
- [ ] PR description explains changes

### PR Description Template

```markdown
## Description
Brief description of changes

## Type of Change
- [ ] Bug fix
- [ ] New feature
- [ ] Breaking change
- [ ] Documentation update

## Testing
Describe tests added/modified

## Checklist
- [ ] Code follows style guidelines
- [ ] Tests pass locally
- [ ] Documentation updated
- [ ] No breaking changes (or documented)

## Related Issues
Fixes #123
```

### Review Process

1. **Automated Checks**: CI must pass
2. **Code Review**: At least one maintainer approval required
3. **Testing**: Reviewer may request additional tests
4. **Documentation**: Ensure docs are clear
5. **Approval**: Maintainer will merge when ready

### After Merge

- Delete your branch
- Update your fork
- Celebrate your contribution!

## Community

### Communication Channels

- **GitHub Issues**: Bug reports, feature requests
- **GitHub Discussions**: General questions, ideas
- **Discord**: Real-time chat, community support
- **Email**: dev@quantenthusiasts.org

### Discord Channels

- `#general`: General discussion
- `#help`: Ask for help
- `#development`: Development discussions
- `#announcements`: Project updates

### Project Roles

**Maintainers**:
- Review and merge pull requests
- Manage releases
- Set project direction

**Contributors**:
- Submit pull requests
- Review others' code
- Help with documentation

**Community Members**:
- Use the project
- Report bugs
- Suggest features

### Becoming a Maintainer

Active contributors may be invited to become maintainers based on:
- Quality of contributions
- Consistency of participation
- Understanding of project goals
- Community engagement

## Recognition

### Contributors

All contributors are recognized in:
- GitHub contributors page
- Release notes
- Project documentation

### Hall of Fame

Outstanding contributors featured on project homepage.

## Additional Resources

- [Development Guide](DEVELOPMENT.md) - Detailed development setup
- [Architecture](ARCHITECTURE.md) - System design
- [API Reference](API.md) - API documentation
- [Deployment Guide](DEPLOYMENT.md) - Production deployment

## Questions?

- **Discord**: [Join community](https://discord.com/invite/z3S9Fguzw3)
- **Issues**: [Ask on GitHub](https://github.com/Quant-Enthusiasts/Quant-Enthusiasts-Risk-Engine/issues)
- **Email**: dev@quantenthusiasts.org

Thank you for contributing to the Quant Enthusiasts Risk Engine!