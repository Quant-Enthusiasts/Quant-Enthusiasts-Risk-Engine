# Quant Enthusiasts Risk Engine

Quant Enthusiasts Risk Engine is a modular, scalable, and cross-language risk engine designed for the Quant Enthusiasts community. It provides a foundation for risk calculation, portfolio management, and real-time data analysis with C++, Python, and JavaScript components.

## Project Structure

```
Quant-Enthusiasts-Risk-Engine/
├── cpp_engine/
│   ├── src/
│   │   ├── utils/
│   │   │   ├── BlackScholes.cpp
│   │   │   └── BlackScholes.h
│   │   ├── Instrument.cpp
│   │   ├── Instrument.h
│   │   ├── MarketData.cpp
│   │   ├── MarketData.h
│   │   ├── Portfolio.cpp
│   │   ├── Portfolio.h
│   │   ├── RiskEngine.cpp
│   │   ├── RiskEngine.h
│   │   └── main.cpp
│   └── CMakeLists.txt
├── python_api/
│   ├── app.py
│   ├── pybind_wrapper.cpp
│   ├── requirements.txt
│   └── setup.py
└── js_dashboard/
    └── index.html
```

## Requirements

* C++17 compatible compiler
* Python 3.11+
* Node.js 20+
* CMake 3.25+

## Installation

Clone the repository:

```bash
git clone https://github.com/Quant-Enthusiasts/Quant-Enthusiasts-Risk-Engine.git
cd Quant-Enthusiasts-Risk-Engine
```

### Build C++ Engine

If you are using GCC or Clang, comment out the line `#include <corecrt_math_defines.h>` in the source file before building. This header is only needed on Windows with MSVC; other compilers already provide the required math constants.

```bash
mkdir -p cpp_engine/build
cd cpp_engine/build
cmake ..
cmake --build .
./risk-engine
```


### Python API

Create a virtual environment and install dependencies, then run the FastAPI server with uvicorn:

```bash
python -m venv venv
# activate (Linux / macOS)
source venv/bin/activate
# activate (Windows PowerShell)
.\venv\Scripts\Activate.ps1
pip install -r python_api/requirements.txt
cd python_api; uvicorn app:app --host 0.0.0.0 --port 5000 --reload
# or using python -m uvicorn
python -m uvicorn app:app --host 0.0.0.0 --port 5000 --reload
```

The FastAPI interactive docs will be available at: http://localhost:5000/docs

### JS Dashboard

Open `js_dashboard/index.html` in your browser or serve it with a local HTTP server:

```bash
cd js_dashboard
npx serve .
```

## Usage

1. C++ engine handles core risk calculations and portfolio management.
2. Python API exposes engine functionality for scripting, automation, and integration.
3. JS Dashboard provides visualization for portfolio risk metrics in real-time.

## Contribution

1. Fork the repository.
2. Create a branch for your feature or bug fix.
3. Commit changes with clear messages.
4. Push to your branch.
5. Open a pull request.

## License

MIT License

## Contact

Quant Enthusiasts Community: [https://discord.com/invite/z3S9Fguzw3](https://discord.com/invite/z3S9Fguzw3)
