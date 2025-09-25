// PM2 Ecosystem Configuration for PM2.5 Ghostbuster v2.1.0
// Enhanced by Claude Code Assistant

module.exports = {
  apps: [
    {
      // Main data collector service
      name: 'pm25-collector',
      script: 'src/main_data_collector.py',
      interpreter: 'python3',
      cwd: __dirname,
      instances: 1,
      exec_mode: 'fork',
      restart_delay: 5000,
      max_restarts: 10,
      min_uptime: '10s',

      // Environment variables
      env: {
        NODE_ENV: 'production',
        PYTHONPATH: '.',
        ENABLE_API: 'true',
        API_PORT: '5000'
      },

      // Logging
      log_file: '/var/log/pm25/pm25-collector.log',
      out_file: '/var/log/pm25/pm25-collector-out.log',
      error_file: '/var/log/pm25/pm25-collector-error.log',
      log_date_format: 'YYYY-MM-DD HH:mm:ss Z',

      // Memory and CPU limits
      max_memory_restart: '500M',

      // Health monitoring
      health_check_url: 'http://localhost:5000/api/v1/health',
      health_check_grace_period: 3000,

      // Auto-restart configuration
      watch: false,  // Set to true for development
      ignore_watch: [
        'node_modules',
        'logs',
        'data',
        '*.log'
      ],

      // Merge logs from all instances
      merge_logs: true,

      // Kill timeout
      kill_timeout: 5000
    },

    {
      // MQTT Logger service (for debugging)
      name: 'pm25-mqtt-logger',
      script: 'src/main_mqtt_logger.py',
      interpreter: 'python3',
      cwd: __dirname,
      instances: 1,
      exec_mode: 'fork',
      restart_delay: 5000,

      // Environment
      env: {
        NODE_ENV: 'production',
        PYTHONPATH: '.'
      },

      // Logging
      log_file: '/var/log/pm25/pm25-mqtt-logger.log',
      out_file: '/var/log/pm25/pm25-mqtt-logger-out.log',
      error_file: '/var/log/pm25/pm25-mqtt-logger-error.log',

      // This service is optional - can be disabled in production
      autorestart: false,

      // Memory limit
      max_memory_restart: '200M'
    },

    {
      // API-only service (if you want to run API separately)
      name: 'pm25-api',
      script: 'scripts/run_api.py',
      interpreter: 'python3',
      cwd: __dirname,
      instances: 1,
      exec_mode: 'fork',

      // Environment
      env: {
        NODE_ENV: 'production',
        PYTHONPATH: '.',
        API_PORT: '5001'  // Different port if running separately
      },

      // Logging
      log_file: '/var/log/pm25/pm25-api.log',
      out_file: '/var/log/pm25/pm25-api-out.log',
      error_file: '/var/log/pm25/pm25-api-error.log',

      // Health check
      health_check_url: 'http://localhost:5001/api/v1/health',

      // This is optional - main collector includes API
      autorestart: false
    }
  ],

  // Deployment configuration
  deploy: {
    production: {
      user: 'pm25',
      host: 'your-server.com',
      ref: 'origin/main',
      repo: 'https://github.com/bejranonda/pm2.5-ghostbuster.git',
      path: '/opt/pm25-ghostbuster',
      'post-deploy': 'pip install -r requirements.txt && pm2 reload ecosystem.config.js --env production',
      'post-setup': 'scripts/setup.sh'
    },

    development: {
      user: 'developer',
      host: 'dev-server.local',
      ref: 'origin/develop',
      repo: 'https://github.com/bejranonda/pm2.5-ghostbuster.git',
      path: '/home/developer/pm25-ghostbuster',
      'post-deploy': 'pip install -r requirements.txt && pm2 reload ecosystem.config.js --env development'
    }
  }
};