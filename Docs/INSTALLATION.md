# Installation Guide

Complete installation instructions for the Quant Enthusiasts Risk Engine across all platforms.

## Prerequisites

### Required

- **C++ Compiler**: GCC 7+, Clang 5+, or MSVC 2017+
- **Python**: 3.11 or higher
- **CMake**: 3.25 or higher
- **Git**: For cloning the repository

### Optional

- **Node.js**: For serving the dashboard locally
- **Docker**: For containerized deployment

## Platform-Specific Setup

### macOS

```bash
# Install Xcode Command Line Tools
xcode-select --install

# Install Homebrew (if not installed)
/bin/bash -c "$(curl -fsSL https://raw.githubusercontent.com/Homebrew/install/HEAD/install.sh)"

# Install dependencies
brew install cmake python@3.11 node

# Install Ninja (optional, faster builds)
brew install ninja
```

### Linux (Ubuntu/Debian)

```bash
# Update package list
sudo apt update

# Install build tools
sudo apt install -y build-essential cmake python3.11 python3.11-venv python3-pip

# Install Ninja (optional)
sudo apt install -y ninja-build

# Install Node.js (optional, for dashboard)
curl -fsSL https://deb.nodesource.com/setup_20.x | sudo -E bash -
sudo apt install -y nodejs
```

### Windows

1. Install [Visual Studio 2019+](https://visualstudio.microsoft.com/) with C++ development tools
2. Install [Python 3.11+](https://www.python.org/downloads/)
3. Install [CMake](https://cmake.org/download/)
4. Install [Git for Windows](https://git-scm.com/download/win)

**Note**: Use PowerShell for all commands below.

## Step-by-Step Installation

### 1. Clone Repository

```bash
git clone https://github.com/Quant-Enthusiasts/Quant-Enthusiasts-Risk-Engine.git
cd Quant-Enthusiasts-Risk-Engine
```

### 2. Build C++ Engine

#### Using Ninja (Recommended)

```bash
cd cpp_engine
mkdir build && cd build
cmake .. -GNinja -DCMAKE_BUILD_TYPE=Release
cmake --build .
cmake --install .
```

#### Using Make (Linux/macOS)

```bash
cd cpp_engine
mkdir build && cd build
cmake .. -DCMAKE_BUILD_TYPE=Release
make -j$(nproc)  # Linux
make -j$(sysctl -n hw.ncpu)  # macOS
cmake --install .
```

#### Using Visual Studio (Windows)

```powershell
cd cpp_engine
mkdir build
cd build
cmake .. -G "Visual Studio 16 2019" -DCMAKE_BUILD_TYPE=Release
cmake --build . --config Release
cmake --install .
```

**Build outputs:**
- `cpp_engine/install/bin/risk-engine` - Standalone C++ demo
- `cpp_engine/install/lib/` - Python module bindings

### 3. Setup Python Environment

```bash
# Navigate to Python API directory
cd python_api

# Create virtual environment
python3 -m venv venv

# Activate virtual environment
source venv/bin/activate        # Linux/macOS
# venv\Scripts\activate         # Windows PowerShell

# Upgrade pip
pip install --upgrade pip

# Install dependencies
pip install -r requirements.txt

# Build Python bindings
python setup.py build_ext --inplace
```

### 4. Verify Installation

#### Test C++ Engine

```bash
cd cpp_engine/install/bin
./risk-engine
```

Expected output: Portfolio risk analysis with pricing comparisons and VaR calculations.

#### Test Python Bindings

```bash
cd ../..  # Return to repo root
python test_installed_module.py
```

Expected output: All tests passing with market data creation, option pricing, and portfolio risk calculations.

#### Test API Server

```bash
cd python_api
export PYTHONPATH="$PWD/../cpp_engine/install/lib:$PYTHONPATH"
python app.py
```

In another terminal:

```bash
curl http://127.0.0.1:5000/health
```

Expected response: JSON with `"status": "healthy"`

### 5. Launch Dashboard (Optional)

```bash
cd js_dashboard

# Option 1: Use npx serve
npx serve .

# Option 2: Use Python HTTP server
python -m http.server 8000
```

Navigate to `http://localhost:3000` (serve) or `http://localhost:8000` (Python).

## Troubleshooting

### Build Issues

#### "CMake cannot find compiler"

**Solution**: Specify compiler explicitly

```bash
cmake -DCMAKE_CXX_COMPILER=g++ ..       # Use GCC
cmake -DCMAKE_CXX_COMPILER=clang++ ..   # Use Clang
```

#### "pybind11 not found"

**Solution**: Install in virtual environment

```bash
pip install pybind11
```

#### "C++17 required" error

**Solution**: Ensure modern compiler or specify standard

```bash
cmake -DCMAKE_CXX_STANDARD=17 ..
```

#### Windows CRLF line ending errors

**Solution**: Convert build scripts to LF

```bash
# Linux/macOS
perl -pi -e 's/\r$//' cpp_engine/build.sh

# Windows (PowerShell)
(Get-Content cpp_engine/build.sh) | Set-Content -NoNewline cpp_engine/build.sh
```

### Runtime Issues

#### "ModuleNotFoundError: quant_risk_engine"

**Solution**: Set PYTHONPATH before running Flask

```bash
export PYTHONPATH="$PWD/cpp_engine/install/lib:$PYTHONPATH"
```

Or create a `.env` file in `python_api/`:

```
PYTHONPATH=../cpp_engine/install/lib
```

#### "Port 5000 already in use" (macOS)

**Cause**: AirPlay Receiver uses port 5000

**Solution**: Use alternative port

```bash
python -m flask --app app run --port 5050
```

Or disable AirPlay Receiver: System Preferences > General > AirDrop & Handoff

#### "Permission denied" on build.sh

**Solution**: Make script executable

```bash
chmod +x cpp_engine/build.sh
```

### Platform-Specific Issues

#### macOS: "stdlib not found"

**Solution**: Comment out Windows-only header

```cpp
// #include <corecrt_math_defines.h>  // Windows only
```

#### Linux: Missing math library

**Solution**: Link math library explicitly

```bash
cmake .. -DCMAKE_CXX_FLAGS="-lm"
```

## Clean Rebuild

If you encounter persistent issues:

```bash
# Clean C++ build
cd cpp_engine
rm -rf build install
mkdir build && cd build
cmake .. -GNinja -DCMAKE_BUILD_TYPE=Release
cmake --build .
cmake --install .

# Clean Python build
cd ../../python_api
rm -rf build dist *.so *.pyd __pycache__
python setup.py build_ext --inplace
```

## Docker Installation

For containerized deployment:

```bash
docker build -t quant-risk-engine .
docker run -p 5000:5000 quant-risk-engine
```

See [DEPLOYMENT.md](DEPLOYMENT.md) for production deployment details.

## Next Steps

- Read [API.md](API.md) for endpoint documentation
- Review [DEVELOPMENT.md](DEVELOPMENT.md) for contribution guidelines
- Check [MARKET_DATA.md](MARKET_DATA.md) for YFinance integration
- Join our [Discord community](https://discord.com/invite/z3S9Fguzw3)

## Getting Help

If you encounter issues not covered here:

1. Check [GitHub Issues](https://github.com/Quant-Enthusiasts/Quant-Enthusiasts-Risk-Engine/issues)
2. Search existing discussions
3. Open a new issue with:
   - Operating system and version
   - Python version (`python --version`)
   - CMake version (`cmake --version`)
   - Compiler version (`g++ --version` or `clang++ --version`)
   - Full error message and stack trace
