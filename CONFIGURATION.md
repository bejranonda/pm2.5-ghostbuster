# PM2.5 Ghostbuster - Configuration Guide

**Version 2.1.0**

Comprehensive configuration guide for PM2.5 Ghostbuster system setup and customization.

## 📋 **Configuration Overview**

PM2.5 Ghostbuster uses environment-based configuration for security and flexibility:

- **Server Configuration**: `.env` files for Python backend
- **Arduino Configuration**: `config.h` files for ESP8266 sensors
- **Process Management**: `ecosystem.config.js` for PM2 deployment

---

## 🖥 **Server Configuration**

### Primary Configuration File: `Server/config/.env`

Copy the example and customize for your environment:

```bash
cp Server/config/.env.example Server/config/.env
nano Server/config/.env
```

### Core Settings

#### MQTT Configuration
```env
# MQTT Broker Settings
MQTT_BROKER_ADDRESS=mqtt.thalay.eu
MQTT_BROKER_PORT=1883
MQTT_USERNAME=pm25
MQTT_PASSWORD=your_secure_mqtt_password
MQTT_TOPIC_PREFIX=pm25
```

**Description:**
- `MQTT_BROKER_ADDRESS`: MQTT broker hostname or IP
- `MQTT_BROKER_PORT`: MQTT broker port (usually 1883 for non-SSL)
- `MQTT_USERNAME/PASSWORD`: Authentication credentials
- `MQTT_TOPIC_PREFIX`: Base topic for all MQTT messages

#### InfluxDB Configuration
```env
# InfluxDB Database Settings
INFLUX_HOST=localhost
INFLUX_PORT=8086
INFLUX_DATABASE=pm25gps
INFLUX_USERNAME=
INFLUX_PASSWORD=
```

**Description:**
- `INFLUX_HOST`: InfluxDB server hostname
- `INFLUX_PORT`: InfluxDB HTTP port (default: 8086)
- `INFLUX_DATABASE`: Database name for air quality data
- Authentication optional for local installations

#### File Paths
```env
# Output Paths
GEOJSON_OUTPUT_PATH=/var/www/html/gj/pm25gps.geojson
CSV_OUTPUT_PATH=/var/www/html/csv/fire.csv
LOG_PATH=/var/log/pm25/
```

**Path Guidelines:**
- Use absolute paths for production
- Ensure write permissions for the service user
- Create directories before starting services

### Enhanced Features (v2.1.0)

#### API Server Settings
```env
# REST API Configuration
ENABLE_API=true
API_PORT=5000
API_HOST=0.0.0.0
```

**Options:**
- `ENABLE_API`: Enable/disable REST API server
- `API_PORT`: Port for API server (default: 5000)
- `API_HOST`: Bind address (0.0.0.0 for all interfaces)

#### Alert System Configuration
```env
# Alert System Settings
MIN_ALERT_LEVEL=unhealthy
ENABLE_EMAIL_ALERTS=false
SMTP_HOST=smtp.gmail.com
SMTP_PORT=587
SMTP_USERNAME=your_email@gmail.com
SMTP_PASSWORD=your_app_password
SMTP_FROM=pm25-ghostbuster@thalay.eu
ALERT_EMAIL=alerts@example.com
```

**Alert Levels:**
- `good` - PM2.5: 0-12 μg/m³
- `moderate` - PM2.5: 12.1-35.4 μg/m³
- `unhealthy_sensitive` - PM2.5: 35.5-55.4 μg/m³
- `unhealthy` - PM2.5: 55.5-150.4 μg/m³ (recommended minimum)
- `very_unhealthy` - PM2.5: 150.5-250.4 μg/m³
- `hazardous` - PM2.5: 250.5+ μg/m³

**Gmail Configuration:**
1. Enable 2-factor authentication
2. Generate app-specific password
3. Use app password in `SMTP_PASSWORD`

#### Data Processing Settings
```env
# Data Management
DATA_RETENTION_HOURS=720
GEOJSON_UPDATE_INTERVAL=60
CSV_UPDATE_INTERVAL=300
```

**Timing Guidelines:**
- `DATA_RETENTION_HOURS`: 720 hours = 30 days (adjust based on storage)
- `GEOJSON_UPDATE_INTERVAL`: Update frequency in seconds
- `CSV_UPDATE_INTERVAL`: Fire data update frequency

#### System Settings
```env
# System Configuration
DEFAULT_TIMEZONE=Asia/Bangkok
DEBUG=false
LOG_LEVEL=INFO
MAP_URL=https://map.thalay.eu
HEALTH_CHECK_INTERVAL=300
STATS_LOG_INTERVAL=300
```

**Debug Levels:**
- `DEBUG` - Detailed debugging information
- `INFO` - General information messages
- `WARNING` - Warning messages only
- `ERROR` - Error messages only

---

## 🔧 **Arduino Configuration**

### Primary Configuration File: `Arduino/config.h`

Create from template and customize:

```bash
cp Arduino/config.h.example Arduino/config.h
```

### Core Arduino Settings

```cpp
// WiFi Configuration
#define WIFI_SSID "your_wifi_network"
#define WIFI_PASSWORD "your_wifi_password"
#define WIFI_CONNECT_TIMEOUT 20000  // 20 seconds

// MQTT Configuration
#define MQTT_BROKER "mqtt.thalay.eu"
#define MQTT_PORT 1883
#define MQTT_USERNAME "pm25"
#define MQTT_PASSWORD "your_mqtt_password"
#define MQTT_CLIENT_ID_PREFIX "pm25-sensor-"
#define MQTT_KEEPALIVE 60
#define MQTT_RECONNECT_INTERVAL 60000  // 60 seconds
```

### Hardware Pin Configuration

```cpp
// Hardware Configuration
#define LED_STATUS_PIN D5
#define GPS_RX_PIN D2
#define GPS_TX_PIN D1
#define GPS_BAUD_RATE 9600

// IKEA VINDRIKTNING Integration
#define VINDRIKTNING_DATA_PIN A0
#define VINDRIKTNING_POWER_PIN D6
```

### Sensor Behavior Settings

```cpp
// Sensor Configuration
#define PM25_SENSOR_BAUD 9600
#define MIN_DISTANCE_METERS 10
#define PING_INTERVAL 3600000  // 1 hour in milliseconds

// Data Publishing
#define PUBLISH_INTERVAL 30000  // 30 seconds
#define GPS_DATA_AGE_THRESHOLD 2000  // 2 seconds
```

**Behavior Tuning:**
- `MIN_DISTANCE_METERS`: Minimum movement to trigger GPS update
- `PUBLISH_INTERVAL`: Regular data transmission interval
- `GPS_DATA_AGE_THRESHOLD`: Maximum age for valid GPS data

---

## 📊 **PM2 Process Configuration**

### Configuration File: `Server/ecosystem.config.js`

Professional process management with PM2:

```javascript
module.exports = {
  apps: [
    {
      name: 'pm25-collector',
      script: 'src/main_data_collector.py',
      interpreter: 'python3',
      instances: 1,
      env: {
        ENABLE_API: 'true',
        API_PORT: '5000'
      },
      log_file: '/var/log/pm25/pm25-collector.log',
      max_memory_restart: '500M',
      health_check_url: 'http://localhost:5000/api/v1/health'
    }
  ]
};
```

### Environment-Specific Configuration

#### Development Environment
```javascript
env_development: {
  NODE_ENV: 'development',
  DEBUG: 'true',
  LOG_LEVEL: 'DEBUG',
  API_PORT: '5000'
}
```

#### Production Environment
```javascript
env_production: {
  NODE_ENV: 'production',
  DEBUG: 'false',
  LOG_LEVEL: 'INFO',
  API_PORT: '5000'
}
```

---

## 🔒 **Security Configuration**

### Secure Credential Management

#### .env File Security
```bash
# Set proper permissions
chmod 600 config/.env
chown your-user:your-group config/.env
```

#### Environment Variables (Alternative)
```bash
# Export environment variables instead of .env file
export MQTT_PASSWORD="your_secure_password"
export INFLUX_PASSWORD="your_db_password"
```

#### Arduino Security
- Never commit `config.h` to version control
- Use strong WiFi passwords
- Consider MQTT over SSL/TLS for production

### Production Security Checklist

- [ ] **Unique passwords** for all services
- [ ] **Network security** with proper firewall rules
- [ ] **HTTPS/SSL** for web interfaces
- [ ] **MQTT authentication** enabled
- [ ] **Database authentication** configured
- [ ] **Log rotation** configured
- [ ] **Regular updates** scheduled

---

## 🌍 **Multi-Environment Setup**

### Development Environment
```env
# Development settings
DEBUG=true
LOG_LEVEL=DEBUG
MQTT_BROKER_ADDRESS=localhost
INFLUX_HOST=localhost
ENABLE_API=true
```

### Production Environment
```env
# Production settings
DEBUG=false
LOG_LEVEL=INFO
MQTT_BROKER_ADDRESS=mqtt.production.thalay.eu
INFLUX_HOST=influx.production.thalay.eu
ENABLE_API=true
ENABLE_EMAIL_ALERTS=true
```

### Testing Environment
```env
# Testing settings
DEBUG=true
LOG_LEVEL=DEBUG
MQTT_BROKER_ADDRESS=mqtt.test.thalay.eu
INFLUX_DATABASE=pm25gps_test
MIN_ALERT_LEVEL=good  # Test all alert levels
```

---

## 📝 **Configuration Validation**

### Automated Validation Script

The setup script includes configuration validation:

```bash
./Server/scripts/setup.sh
```

### Manual Validation

#### Test Configuration Loading
```python
from config.settings import config
print("✅ Configuration loaded successfully")
print(f"MQTT Broker: {config.MQTT_BROKER_ADDRESS}")
print(f"API Enabled: {getattr(config, 'ENABLE_API', False)}")
```

#### Test Database Connection
```bash
# Test InfluxDB connection
influx -host localhost -port 8086 -execute "SHOW DATABASES"

# Test MQTT connection
mosquitto_pub -h mqtt.thalay.eu -t test/message -m "hello"
```

---

## 🔧 **Troubleshooting Configuration**

### Common Issues

#### MQTT Connection Failed
```bash
# Check MQTT broker connectivity
telnet mqtt.thalay.eu 1883

# Test with mosquitto client
mosquitto_sub -h mqtt.thalay.eu -t "pm25/+/air" -u pm25 -P your_password
```

#### InfluxDB Connection Issues
```bash
# Check InfluxDB service
systemctl status influxdb

# Test HTTP interface
curl -i 'http://localhost:8086/ping'
```

#### Permission Errors
```bash
# Fix log directory permissions
sudo mkdir -p /var/log/pm25
sudo chown $USER:$USER /var/log/pm25

# Fix data directory permissions
sudo mkdir -p /var/www/html/gj /var/www/html/csv
sudo chown $USER:$USER /var/www/html/gj /var/www/html/csv
```

### Configuration Debug Commands

```bash
# Check environment variables
env | grep -E "(MQTT|INFLUX|API)"

# Validate configuration syntax
python3 -c "from config.settings import config; print('Config OK')"

# Test health check
curl http://localhost:5000/api/v1/health
```

---

## 🎯 **Best Practices**

### Configuration Management
1. **Version Control**: Use `.env.example` templates
2. **Environment Separation**: Different configs for dev/test/prod
3. **Secret Management**: Never commit passwords
4. **Documentation**: Document all configuration options

### Performance Tuning
1. **Database Retention**: Adjust based on storage capacity
2. **Update Intervals**: Balance between real-time and performance
3. **Alert Thresholds**: Set appropriate levels for your use case
4. **Resource Limits**: Configure memory and CPU limits

### Monitoring
1. **Health Checks**: Regular system health monitoring
2. **Log Rotation**: Prevent log files from growing too large
3. **Alert Testing**: Test email notifications regularly
4. **Performance Metrics**: Monitor system resource usage

---

## 🎯 **Configuration Excellence**

This configuration system implements professional development principles:

- **Security First**: Environment-based credential management
- **Flexibility**: Multi-environment support with validation
- **Documentation**: Comprehensive guides with examples
- **Best Practices**: Industry-standard configuration patterns
- **Automation**: Validation and setup scripts

---

**Making configuration management invisible and effortless!** 👻

For additional help, see:
- [DEPLOYMENT.md](DEPLOYMENT.md) - Complete deployment guide
- [API.md](API.md) - REST API documentation
- [README.md](README.md) - Project overview