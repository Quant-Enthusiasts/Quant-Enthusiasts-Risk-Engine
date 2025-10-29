import React from "react";

export const DeploymentSection: React.FC = () => (
  <div className="space-y-6">
    <h2 className="text-3xl font-bold text-cyan-400 mb-6">Deployment</h2>

    <div className="mb-8">
      <h3 className="text-xl font-bold text-gray-100 mb-4">
        Docker Deployment
      </h3>
      <p className="text-gray-300 mb-4">
        Containerized deployment for consistent environments.
      </p>

      <h4 className="text-sm font-semibold text-gray-300 mb-2">
        Using Pre-Built Image
      </h4>
      <div className="code-block mb-4" style={{ whiteSpace: "pre" }}>
        {`# Pull latest image
docker pull quantenthusiasts/risk-engine:latest

# Run container
docker run -d \\
  --name risk-engine \\
  -p 5000:5000 \\
  -e FLASK_ENV=production \\
  -v $(pwd)/data:/app/data \\
  quantenthusiasts/risk-engine:latest`}
      </div>

      <h4 className="text-sm font-semibold text-gray-300 mb-2">
        Docker Compose
      </h4>
      <div className="code-block" style={{ whiteSpace: "pre" }}>
        {`version: '3.8'
services:
  api:
    build: .
    ports:
      - "5000:5000"
    environment:
      - FLASK_ENV=production
      - MARKET_DATA_CACHE_HOURS=24
    volumes:
      - ./data:/app/data
    restart: unless-stopped`}
      </div>
    </div>

    <div className="mb-8">
      <h3 className="text-xl font-bold text-gray-100 mb-4">Cloud Platforms</h3>

      <div className="space-y-4">
        <div className="bg-gray-800/40 p-5 rounded-lg border border-gray-700/60">
          <h4 className="text-cyan-400 font-bold mb-2">AWS ECS</h4>
          <p className="text-sm text-gray-400 mb-2">
            Deploy using Elastic Container Service with Fargate
          </p>
          <div className="code-block text-xs" style={{ whiteSpace: "pre" }}>
            {`aws ecs create-service \\
  --cluster risk-engine-cluster \\
  --service-name risk-engine-service \\
  --task-definition risk-engine \\
  --desired-count 2 \\
  --launch-type FARGATE`}
          </div>
        </div>

        <div className="bg-gray-800/40 p-5 rounded-lg border border-gray-700/60">
          <h4 className="text-cyan-400 font-bold mb-2">Google Cloud Run</h4>
          <p className="text-sm text-gray-400 mb-2">
            Serverless container deployment
          </p>
          <div className="code-block text-xs" style={{ whiteSpace: "pre" }}>
            {`gcloud run deploy risk-engine \\
  --image gcr.io/PROJECT-ID/risk-engine \\
  --platform managed \\
  --allow-unauthenticated`}
          </div>
        </div>

        <div className="bg-gray-800/40 p-5 rounded-lg border border-gray-700/60">
          <h4 className="text-cyan-400 font-bold mb-2">DigitalOcean</h4>
          <p className="text-sm text-gray-400 mb-2">App Platform deployment</p>
          <div className="code-block text-xs" style={{ whiteSpace: "pre" }}>
            {`doctl apps create --spec app.yaml`}
          </div>
        </div>
      </div>
    </div>

    <div className="mb-8">
      <h3 className="text-xl font-bold text-gray-100 mb-4">
        Production Configuration
      </h3>

      <h4 className="text-sm font-semibold text-gray-300 mb-2">
        Environment Variables
      </h4>
      <div className="code-block mb-4" style={{ whiteSpace: "pre" }}>
        {`# Application
FLASK_ENV=production
SECRET_KEY=your-secret-key

# Market Data
MARKET_DATA_CACHE_HOURS=24
DEFAULT_RISK_FREE_RATE=0.045

# Performance
WORKERS=4
MAX_CONTENT_LENGTH=16777216

# Logging
LOG_LEVEL=INFO
LOG_FILE=/app/logs/risk-engine.log`}
      </div>

      <h4 className="text-sm font-semibold text-gray-300 mb-2">
        Security Best Practices
      </h4>
      <ul className="text-sm text-gray-400 space-y-2">
        <li>• Implement API authentication (API keys or OAuth2)</li>
        <li>• Enable HTTPS with TLS certificates</li>
        <li>• Configure rate limiting per IP/API key</li>
        <li>• Use environment variables for secrets</li>
        <li>• Enable CORS only for trusted domains</li>
        <li>• Set up logging and monitoring</li>
      </ul>
    </div>
  </div>
);


