#!/bin/bash
# start.sh - Launch Quant Enthusiasts Risk Engine (Python API + JS Dashboard)

set -e  # Exit immediately if a command fails

echo "Starting Quant Enthusiasts Risk Engine..."

# -----------------------------
# 1. Python API
# -----------------------------
echo "Setting up Python API..."
cd python_api

# Create virtual environment if it doesn't exist
if [ ! -d "venv" ]; then
    echo "Creating Python virtual environment..."
    python3 -m venv venv
fi

# Activate virtual environment
source venv/bin/activate

# Install dependencies
pip install --upgrade pip
pip install -r requirements.txt

# Build Python bindings
python3 setup.py build_ext --inplace

cd ..

# -----------------------------
# 2. JS Dashboard
# -----------------------------
echo "Setting up JS Dashboard..."
cd js_dashboard

# Install Node dependencies if package.json exists
if [ -f "package.json" ]; then
    npm install
fi

# Start local server
echo "Launching JS Dashboard..."
npx serve . -p 3000 &

cd ..

# -----------------------------
# 3. Launch API in foreground
# -----------------------------
echo "Launching Python API..."
cd python_api
exec python app.py
