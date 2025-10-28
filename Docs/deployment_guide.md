# Deployment Guide

Production deployment instructions for the Quant Enthusiasts Risk Engine.

## Overview

This guide covers deploying the Risk Engine in production environments using Docker, cloud platforms, and traditional server setups.

## Docker Deployment

### Using Pre-Built Image

```bash
# Pull latest image
docker pull quantenthusiasts/risk-engine:latest

# Run container
docker run -d \
  --name risk-engine \
  -p 5000:5000 \
  -e FLASK_ENV=production \
  -v $(pwd)/data:/app/data \
  quantenthusiasts/risk-engine:latest
```

### Building Custom Image

**Dockerfile** (included in repository):
```dockerfile
FROM python:3.11-slim

WORKDIR /app

# Install system dependencies
RUN apt-get update && apt-get install -y \
    build-essential \
    cmake \
    ninja-build \
    && rm -rf /var/lib/apt/lists/*

# Copy project files
COPY cpp_engine/ ./cpp_engine/
COPY python_api/ ./python_api/
COPY js_dashboard/ ./js_dashboard/

# Build C++ engine
RUN cd cpp_engine && \
    mkdir build && cd build && \
    cmake .. -GNinja -DCMAKE_BUILD_TYPE=Release && \
    cmake --build . && \
    cmake --install .

# Install Python dependencies
RUN cd python_api && \
    pip install --no-cache-dir -r requirements.txt && \
    python setup.py build_ext --inplace

# Set environment
ENV PYTHONPATH=/app/cpp_engine/install/lib:$PYTHONPATH
ENV FLASK_APP=python_api/app.py

EXPOSE 5000

CMD ["python", "-m", "flask", "run", "--host=0.0.0.0"]
```

**Build and run**:
```bash
# Build image
docker build -t risk-engine:custom .

# Run with environment variables
docker run -d \
  --name risk-engine \
  -p 5000:5000 \
  -e MARKET_DATA_CACHE_HOURS=12 \
  -e DEFAULT_RISK_FREE_RATE=0.045 \
  -v risk-engine-data:/app/data \
  risk-engine:custom
```

### Docker Compose

**docker-compose.yml**:
```yaml
version: '3.8'

services:
  api:
    build: .
    container_name: risk-engine-api
    ports:
      - "5000:5000"
    environment:
      - FLASK_ENV=production
      - MARKET_DATA_CACHE_HOURS=24
      - DEFAULT_RISK_FREE_RATE=0.045
    volumes:
      - ./data:/app/data
      - ./logs:/app/logs
    restart: unless-stopped
    healthcheck:
      test: ["CMD", "curl", "-f", "http://localhost:5000/health"]
      interval: 30s
      timeout: 10s
      retries: 3

  dashboard:
    image: nginx:alpine
    container_name: risk-engine-dashboard
    ports:
      - "80:80"
    volumes:
      - ./js_dashboard:/usr/share/nginx/html:ro
      - ./nginx.conf:/etc/nginx/nginx.conf:ro
    depends_on:
      - api
    restart: unless-stopped
```

**nginx.conf** (for dashboard):
```nginx
events {
    worker_connections 1024;
}

http {
    include /etc/nginx/mime.types;
    
    server {
        listen 80;
        server_name localhost;
        
        # Dashboard
        location / {
            root /usr/share/nginx/html;
            index index.html;
        }
        
        # Proxy API requests
        location /api/ {
            proxy_pass http://api:5000/;
            proxy_set_header Host $host;
            proxy_set_header X-Real-IP $remote_addr;
        }
    }
}
```

**Deploy with Compose**:
```bash
docker-compose up -d
docker-compose logs -f api
```

## Cloud Deployment

### AWS Elastic Container Service (ECS)

**Task Definition** (JSON):
```json
{
  "family": "risk-engine",
  "containerDefinitions": [
    {
      "name": "api",
      "image": "quantenthusiasts/risk-engine:latest",
      "memory": 2048,
      "cpu": 1024,
      "essential": true,
      "portMappings": [
        {
          "containerPort": 5000,
          "protocol": "tcp"
        }
      ],
      "environment": [
        {"name": "FLASK_ENV", "value": "production"},
        {"name": "MARKET_DATA_CACHE_HOURS", "value": "24"}
      ],
      "logConfiguration": {
        "logDriver": "awslogs",
        "options": {
          "awslogs-group": "/ecs/risk-engine",
          "awslogs-region": "us-east-1",
          "awslogs-stream-prefix": "api"
        }
      }
    }
  ],
  "requiresCompatibilities": ["FARGATE"],
  "networkMode": "awsvpc",
  "cpu": "1024",
  "memory": "2048"
}
```

**Deploy to ECS**:
```bash
# Create cluster
aws ecs create-cluster --cluster-name risk-engine-cluster

# Register task definition
aws ecs register-task-definition --cli-input-json file://task-definition.json

# Create service
aws ecs create-service \
  --cluster risk-engine-cluster \
  --service-name risk-engine-service \
  --task-definition risk-engine \
  --desired-count 2 \
  --launch-type FARGATE \
  --network-configuration "awsvpcConfiguration={subnets=[subnet-xxx],securityGroups=[sg-xxx],assignPublicIp=ENABLED}"
```

### Google Cloud Run

```bash
# Build and push image
gcloud builds submit --tag gcr.io/PROJECT-ID/risk-engine

# Deploy to Cloud Run
gcloud run deploy risk-engine \
  --image gcr.io/PROJECT-ID/risk-engine \
  --platform managed \
  --region us-central1 \
  --allow-unauthenticated \
  --memory 2Gi \
  --cpu 2 \
  --set-env-vars FLASK_ENV=production,MARKET_DATA_CACHE_HOURS=24
```

### Azure Container Instances

```bash
# Create resource group
az group create --name risk-engine-rg --location eastus

# Create container instance
az container create \
  --resource-group risk-engine-rg \
  --name risk-engine \
  --image quantenthusiasts/risk-engine:latest \
  --cpu 2 \
  --memory 4 \
  --ports 5000 \
  --environment-variables FLASK_ENV=production \
  --dns-name-label risk-engine-app
```

### DigitalOcean App Platform

**app.yaml**:
```yaml
name: risk-engine
services:
  - name: api
    github:
      repo: Quant-Enthusiasts/Quant-Enthusiasts-Risk-Engine
      branch: main
      deploy_on_push: true
    dockerfile_path: Dockerfile
    http_port: 5000
    instance_count: 2
    instance_size_slug: professional-xs
    envs:
      - key: FLASK_ENV
        value: production
      - key: MARKET_DATA_CACHE_HOURS
        value: "24"
```

```bash
# Deploy using doctl
doctl apps create --spec app.yaml
```

## Traditional Server Deployment

### Using systemd (Linux)

**1. Setup application**:
```bash
# Create app user
sudo useradd -r -s /bin/false riskengine

# Clone repository
sudo mkdir -p /opt/risk-engine
sudo chown riskengine:riskengine /opt/risk-engine
cd /opt/risk-engine
git clone https://github.com/Quant-Enthusiasts/Quant-Enthusiasts-Risk-Engine.git .

# Build and install (as riskengine user)
sudo -u riskengine bash << 'EOF'
# Build C++ engine
cd cpp_engine
mkdir build && cd build
cmake .. -GNinja -DCMAKE_BUILD_TYPE=Release
cmake --build .
cmake --install .

# Setup Python environment
cd ../../python_api
python3 -m venv venv
source venv/bin/activate
pip install -r requirements.txt
python setup.py build_ext --inplace
EOF
```

**2. Create systemd service**:

`/etc/systemd/system/risk-engine.service`:
```ini
[Unit]
Description=Quant Enthusiasts Risk Engine API
After=network.target

[Service]
Type=simple
User=riskengine
Group=riskengine
WorkingDirectory=/opt/risk-engine/python_api
Environment="PYTHONPATH=/opt/risk-engine/cpp_engine/install/lib"
Environment="FLASK_ENV=production"
ExecStart=/opt/risk-engine/python_api/venv/bin/python -m flask run --host=0.0.0.0 --port=5000
Restart=on-failure
RestartSec=10

[Install]
WantedBy=multi-user.target
```

**3. Enable and start service**:
```bash
sudo systemctl daemon-reload
sudo systemctl enable risk-engine
sudo systemctl start risk-engine
sudo systemctl status risk-engine
```

### Using Gunicorn (Production WSGI)

**1. Install Gunicorn**:
```bash
cd python_api
source venv/bin/activate
pip install gunicorn
```

**2. Create Gunicorn configuration**:

`gunicorn_config.py`:
```python
import multiprocessing

bind = "0.0.0.0:5000"
workers = multiprocessing.cpu_count() * 2 + 1
worker_class = "sync"
timeout = 120
keepalive = 5
errorlog = "/var/log/risk-engine/error.log"
accesslog = "/var/log/risk-engine/access.log"
loglevel = "info"
```

**3. Update systemd service**:
```ini
[Service]
ExecStart=/opt/risk-engine/python_api/venv/bin/gunicorn \
  -c /opt/risk-engine/python_api/gunicorn_config.py \
  app:app
```

### Nginx Reverse Proxy

**nginx configuration** (`/etc/nginx/sites-available/risk-engine`):
```nginx
upstream risk_engine {
    server 127.0.0.1:5000;
}

server {
    listen 80;
    server_name api.example.com;
    
    # Redirect to HTTPS
    return 301 https://$server_name$request_uri;
}

server {
    listen 443 ssl http2;
    server_name api.example.com;
    
    ssl_certificate /etc/letsencrypt/live/api.example.com/fullchain.pem;
    ssl_certificate_key /etc/letsencrypt/live/api.example.com/privkey.pem;
    
    # SSL configuration
    ssl_protocols TLSv1.2 TLSv1.3;
    ssl_ciphers HIGH:!aNULL:!MD5;
    ssl_prefer_server_ciphers on;
    
    # Logging
    access_log /var/log/nginx/risk-engine-access.log;
    error_log /var/log/nginx/risk-engine-error.log;
    
    # Rate limiting
    limit_req_zone $binary_remote_addr zone=api:10m rate=10r/s;
    limit_req zone=api burst=20 nodelay;
    
    location / {
        proxy_pass http://risk_engine;
        proxy_set_header Host $host;
        proxy_set_header X-Real-IP $remote_addr;
        proxy_set_header X-Forwarded-For $proxy_add_x_forwarded_for;
        proxy_set_header X-Forwarded-Proto $scheme;
        
        # Timeouts
        proxy_connect_timeout 60s;
        proxy_send_timeout 60s;
        proxy_read_timeout 60s;
    }
    
    # Health check endpoint
    location /health {
        proxy_pass http://risk_engine/health;
        access_log off;
    }
}
```

**Enable configuration**:
```bash
sudo ln -s /etc/nginx/sites-available/risk-engine /etc/nginx/sites-enabled/
sudo nginx -t
sudo systemctl reload nginx
```

## Production Configuration

### Environment Variables

Create `.env` file in `python_api/`:
```bash
# Application
FLASK_ENV=production
SECRET_KEY=your-secret-key-here

# Market Data
MARKET_DATA_CACHE_HOURS=24
DEFAULT_RISK_FREE_RATE=0.045
MARKET_DATA_CACHE_PATH=/app/data/market_data_cache.db

# Logging
LOG_LEVEL=INFO
LOG_FILE=/app/logs/risk-engine.log

# Performance
WORKERS=4
MAX_CONTENT_LENGTH=16777216  # 16MB
```

### Logging Configuration

**Python logging** (`logging_config.py`):
```python
import logging
import os
from logging.handlers import RotatingFileHandler

def setup_logging(app):
    log_level = os.getenv('LOG_LEVEL', 'INFO')
    log_file = os.getenv('LOG_FILE', 'risk-engine.log')
    
    handler = RotatingFileHandler(
        log_file,
        maxBytes=10485760,  # 10MB
        backupCount=10
    )
    
    formatter = logging.Formatter(
        '%(asctime)s %(levelname)s [%(name)s] %(message)s'
    )
    handler.setFormatter(formatter)
    
    app.logger.addHandler(handler)
    app.logger.setLevel(getattr(logging, log_level))
```

### Security Best Practices

1. **API Authentication**:
```python
from functools import wraps
from flask import request, jsonify

def require_api_key(f):
    @wraps(f)
    def decorated_function(*args, **kwargs):
        api_key = request.headers.get('X-API-Key')
        if not api_key or api_key != os.getenv('API_KEY'):
            return jsonify({'error': 'Unauthorized'}), 401
        return f(*args, **kwargs)
    return decorated_function

@app.route('/calculate_risk', methods=['POST'])
@require_api_key
def calculate_risk():
    # Implementation
    pass
```

2. **CORS Configuration**:
```python
from flask_cors import CORS

CORS(app, resources={
    r"/api/*": {
        "origins": ["https://dashboard.example.com"],
        "methods": ["GET", "POST"],
        "allow_headers": ["Content-Type", "X-API-Key"]
    }
})
```

3. **Rate Limiting**:
```python
from flask_limiter import Limiter
from flask_limiter.util import get_remote_address

limiter = Limiter(
    app,
    key_func=get_remote_address,
    default_limits=["100 per hour"]
)

@app.route('/calculate_risk', methods=['POST'])
@limiter.limit("10 per minute")
def calculate_risk():
    pass
```

## Monitoring

### Health Checks

**Kubernetes liveness/readiness probes**:
```yaml
livenessProbe:
  httpGet:
    path: /health
    port: 5000
  initialDelaySeconds: 30
  periodSeconds: 10

readinessProbe:
  httpGet:
    path: /health
    port: 5000
  initialDelaySeconds: 5
  periodSeconds: 5
```

**Custom health endpoint**:
```python
@app.route('/health/detailed', methods=['GET'])
def health_detailed():
    checks = {
        'api': 'healthy',
        'cache': check_cache_connection(),
        'disk_space': check_disk_space(),
        'memory': check_memory_usage()
    }
    
    status = 'healthy' if all(v == 'healthy' for v in checks.values()) else 'degraded'
    status_code = 200 if status == 'healthy' else 503
    
    return jsonify({
        'status': status,
        'checks': checks,
        'timestamp': datetime.now().isoformat()
    }), status_code
```

### Application Monitoring

**Prometheus metrics**:
```python
from prometheus_client import Counter, Histogram, generate_latest

request_count = Counter(
    'http_requests_total',
    'Total HTTP requests',
    ['method', 'endpoint', 'status']
)

request_duration = Histogram(
    'http_request_duration_seconds',
    'HTTP request duration',
    ['method', 'endpoint']
)

@app.route('/metrics')
def metrics():
    return generate_latest()
```

### Log Aggregation

**Fluentd configuration**:
```yaml
<source>
  @type tail
  path /var/log/risk-engine/*.log
  pos_file /var/log/td-agent/risk-engine.pos
  tag risk-engine
  <parse>
    @type json
  </parse>
</source>

<match risk-engine>
  @type elasticsearch
  host elasticsearch
  port 9200
  logstash_format true
  logstash_prefix risk-engine
</match>
```

## Backup and Disaster Recovery

### Database Backup

```bash
# Backup market data cache
#!/bin/bash
BACKUP_DIR="/backups/risk-engine"
DATE=$(date +%Y%m%d_%H%M%S)

sqlite3 /app/data/market_data_cache.db ".backup $BACKUP_DIR/cache_$DATE.db"

# Keep last 7 days
find $BACKUP_DIR -name "cache_*.db" -mtime +7 -delete
```

### Automated Backup (cron):
```cron
0 2 * * * /opt/risk-engine/scripts/backup.sh
```

## Performance Tuning

### Application Level

1. **Worker Configuration**:
```python
# gunicorn_config.py
workers = (2 * cpu_count) + 1
worker_class = 'gevent'  # For I/O bound operations
worker_connections = 1000
```

2. **Connection Pooling**:
```python
from sqlalchemy import create_engine
from sqlalchemy.pool import QueuePool

engine = create_engine(
    'sqlite:///market_data_cache.db',
    poolclass=QueuePool,
    pool_size=10,
    max_overflow=20
)
```

### System Level

1. **Increase file descriptors**:
```bash
# /etc/security/limits.conf
riskengine soft nofile 65536
riskengine hard nofile 65536
```

2. **TCP optimization**:
```bash
# /etc/sysctl.conf
net.core.somaxconn = 1024
net.ipv4.tcp_max_syn_backlog = 2048
```

## Troubleshooting Production Issues

### Common Issues

**High Memory Usage**:
```bash
# Check process memory
ps aux | grep risk-engine

# Restart service
sudo systemctl restart risk-engine
```

**Connection Timeouts**:
```bash
# Check network connectivity
curl -I https://query1.finance.yahoo.com

# Increase timeout in configuration
```

**Cache Corruption**:
```bash
# Backup and recreate cache
cp market_data_cache.db market_data_cache.db.backup
sqlite3 market_data_cache.db "VACUUM;"
```

### Log Analysis

```bash
# Error frequency
grep ERROR /var/log/risk-engine/error.log | wc -l

# Most common errors
grep ERROR /var/log/risk-engine/error.log | awk '{print $NF}' | sort | uniq -c | sort -nr | head -10

# Request rate
tail -f /var/log/risk-engine/access.log | pv -l > /dev/null
```

## Scaling

### Horizontal Scaling

**Load balancer configuration** (HAProxy):
```
backend risk_engine
    balance roundrobin
    option httpchk GET /health
    server api1 10.0.1.10:5000 check
    server api2 10.0.1.11:5000 check
    server api3 10.0.1.12:5000 check
```

### Caching Strategy

**Redis for shared cache**:
```python
import redis
from flask_caching import Cache

cache = Cache(config={
    'CACHE_TYPE': 'redis',
    'CACHE_REDIS_HOST': 'redis',
    'CACHE_REDIS_PORT': 6379,
    'CACHE_DEFAULT_TIMEOUT': 300
})

@app.route('/calculate_risk', methods=['POST'])
@cache.cached(timeout=60)
def calculate_risk():
    pass
```

## See Also

- [Installation Guide](INSTALLATION.md) - Initial setup
- [API Reference](API.md) - Endpoint documentation
- [Development Guide](DEVELOPMENT.md) - Contributing guidelines