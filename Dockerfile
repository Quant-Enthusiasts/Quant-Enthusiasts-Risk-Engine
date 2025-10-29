# -----------------------------
# Base image
# -----------------------------
FROM python:3.11-slim

# -----------------------------
# Set working directory
# -----------------------------
WORKDIR /app

# -----------------------------
# Install system dependencies
# -----------------------------
RUN apt-get update && \
    apt-get install -y --no-install-recommends \
        nodejs \
        npm \
        build-essential \
        curl \
        xsel \
    && apt-get clean \
    && rm -rf /var/lib/apt/lists/*

# -----------------------------
# Copy project files
# -----------------------------
COPY . /app

# -----------------------------
# Python setup
# -----------------------------
# Create virtual environment
RUN python3 -m venv venv

# Upgrade pip and install Python dependencies
RUN ./venv/bin/pip install --upgrade pip && \
    ./venv/bin/pip install -r python_api/requirements.txt && \
    ./venv/bin/pip install pybind11

# Build Python bindings
WORKDIR /app/python_api
RUN ../venv/bin/python setup.py install
WORKDIR /app

# -----------------------------
# React Dashboard setup
# -----------------------------
WORKDIR /app/react_dashboard
# Install dependencies and build
RUN if [ -f package.json ]; then npm install && npm run build; fi
WORKDIR /app

# -----------------------------
# Environment variables
# -----------------------------
ENV PATH="/app/venv/bin:$PATH"
# React Dashboard environment variables
ENV VITE_API_BASE_URL="https://quant-enthusiasts-risk-engine.onrender.com/"
ENV VITE_API_HEALTH_CHECK_INTERVAL="30000"

# -----------------------------
# Expose ports
# -----------------------------
EXPOSE 5000
EXPOSE 3000

# -----------------------------
# Start script
# -----------------------------
CMD ["bash", "start.sh"]
