# PM2.5 Ghostbuster - Deployment Guide

This guide covers the deployment of the enhanced PM2.5 Ghostbuster system v2.1.0 with advanced features, alerts, and automation.

## 🏗 Architecture Overview (v2.1.0)

The enhanced system features advanced monitoring, alerting, and automation:

```
PM25-Ghostbuster/
├── Arduino/                    # IKEA VINDRIKTNING-based sensors
│   ├── pm25_ghostbuster.ino   # Enhanced main sketch
│   ├── config.h.example       # Secure configuration template
│   ├── wifi_manager.*         # WiFi management with auto-recovery
│   └── mqtt_client.*          # MQTT client with guaranteed delivery
├── Server/                     # Enterprise-grade backend (v2.1.0)
│   ├── config/                # Enhanced configuration management
│   ├── src/                   # Modular source code
│   │   ├── services/          # Business logic services
│   │   │   ├── alert_service.py    # 🆕 WHO/EPA alert system
│   │   │   └── api_service.py      # 🆕 REST API
│   │   ├── models/            # Data models with validation
│   │   └── utils/             # Enhanced utilities
│   ├── scripts/               # 🆕 Automation tools
│   │   ├── setup.sh          # One-command setup
│   │   ├── health_check.py   # System monitoring
│   │   └── run_api.py        # Standalone API server
│   ├── ecosystem.config.js    # 🆕 PM2 configuration
│   └── requirements.txt       # Enhanced dependencies
└── Leaflet/                   # Web interface with API integration
```

### 🆕 **New Features in v2.1.0**
- **Advanced Alert System**: WHO/EPA PM2.5 thresholds with email notifications
- **REST API**: Complete HTTP API for system integration
- **Automation Tools**: One-command setup and health monitoring
- **Enhanced Monitoring**: System resource and performance tracking
- **Professional Deployment**: PM2 ecosystem with process management

## 🔧 Server Deployment

### Prerequisites

- Python 3.8+
- InfluxDB 1.x
- MQTT Broker (Mosquitto recommended)
- Web server (Apache/Nginx for static files)

### Step 1: Environment Setup

```bash
# Clone repository
git clone https://github.com/bejranonda/pm2.5-ghostbuster.git
cd pm2.5-ghostbuster/Server

# Install Python dependencies
pip install -r requirements.txt

# Create configuration
cp config/.env.example config/.env
```

### Step 2: Configuration

Edit `config/.env` with your settings:

```env
# MQTT Configuration
MQTT_BROKER_ADDRESS=mqtt.thalay.eu
MQTT_BROKER_PORT=1883
MQTT_USERNAME=pm25
MQTT_PASSWORD=your_secure_password

# InfluxDB Configuration
INFLUX_HOST=localhost
INFLUX_PORT=8086
INFLUX_DATABASE=pm25gps
INFLUX_USERNAME=your_db_user
INFLUX_PASSWORD=your_db_password

# File Paths
GEOJSON_OUTPUT_PATH=/var/www/html/gj/pm25gps.geojson
LOG_PATH=/var/log/pm25/

# Data Processing
DATA_RETENTION_HOURS=720
GEOJSON_UPDATE_INTERVAL=60
```

### Step 3: Database Setup

```bash
# Install and start InfluxDB
sudo systemctl start influxdb
sudo systemctl enable influxdb

# Create database
influx -execute "CREATE DATABASE pm25gps"
```

### Step 4: Service Deployment

Using PM2 for process management:

```bash
# Install PM2
npm install -g pm2

# Start services
pm2 start src/main_data_collector.py --name pm25-collector --interpreter python3
pm2 start src/main_mqtt_logger.py --name pm25-logger --interpreter python3

# Save PM2 configuration
pm2 save
pm2 startup
```

### Step 5: Web Server Setup

For Apache:

```apache
<VirtualHost *:80>
    ServerName map.thalay.eu
    DocumentRoot /var/www/html

    # Enable CORS for API access
    Header always set Access-Control-Allow-Origin "*"
    Header always set Access-Control-Allow-Methods "GET, POST, OPTIONS"
    Header always set Access-Control-Allow-Headers "Content-Type"

    # Cache static assets
    <LocationMatch "\\.(css|js|png|jpg|jpeg|gif|ico|svg)$">
        ExpiresActive On
        ExpiresDefault "access plus 1 month"
    </LocationMatch>

    # Dynamic data should not be cached
    <LocationMatch "\\.(json|geojson)$">
        ExpiresActive On
        ExpiresDefault "access plus 1 minute"
    </LocationMatch>
</VirtualHost>
```

## 📟 Arduino Deployment

### Step 1: IKEA VINDRIKTNING Hardware Setup

#### Required Components
- **IKEA VINDRIKTNING** Air Quality Sensor (Product ID: 804.982.46)
  - Available at [India IKEA](https://www.ikea.com/in/en/p/vindriktning-air-quality-sensor-80498246/)
  - Alternative sources: eBay, Amazon, local electronics stores
- **ESP8266 development board** (NodeMCU, Wemos D1 Mini)
- **GPS module** (Neo-6M recommended)
- **Status LED** and resistor
- **Breadboard and jumper wires**
- **USB-C cable and power adapter** (for VINDRIKTNING)

#### VINDRIKTNING Modification
The IKEA VINDRIKTNING sensor needs to be opened to access the PM2.5 data:

1. **Disassemble** the VINDRIKTNING housing (4 screws on the back)
2. **Locate** the sensor board inside
3. **Identify** the data pins for PM2.5 readings
4. **Connect** ESP8266 to read sensor data
5. **Maintain** original USB-C power connection

#### Enhanced Wiring (v2.1.0)
```
ESP8266 Pin    Component
-----------    ---------
D1 (TX)     -> GPS RX
D2 (RX)     -> GPS TX
D5          -> Status LED (+ resistor)
A0          -> VINDRIKTNING data pin
VIN         -> External components power
GND         -> Common ground (including VINDRIKTNING)
3.3V        -> GPS module power
```

> **💡 Pro Tip**: The VINDRIKTNING's original functionality remains intact - it will continue to show green/yellow/red indicators while also transmitting data via our ESP8266 enhancement.

### Step 2: Software Setup

```bash
# Install Arduino IDE and libraries:
# - TinyGPS++
# - PubSubClient
# - ESP8266WiFi (included with ESP8266 board package)
```

### Step 3: Configuration

```cpp
// Copy config.h.example to config.h and edit:
#define WIFI_SSID "your_wifi_network"
#define WIFI_PASSWORD "your_wifi_password"
#define MQTT_PASSWORD "your_mqtt_password"
```

### Step 4: Upload and Deploy

1. Select board: ESP8266 (your specific model)
2. Select port: COM port of your ESP8266
3. Upload the sketch
4. Monitor serial output for connection status

## 🖥 Frontend Deployment

### Static File Deployment

Copy Leaflet files to web server:

```bash
# Copy to web server directory
cp -r Leaflet/* /var/www/html/

# Set appropriate permissions
chown -R www-data:www-data /var/www/html
chmod -R 755 /var/www/html
```

### Configuration

Edit `Leaflet/index.html` to point to your data sources:

```javascript
// Update data source URLs
const DATA_SOURCE = '/gj/pm25gps.geojson';
const CSV_SOURCE = '/csv/fire.csv';
```

## 🔍 Monitoring and Maintenance

### Health Checks

Monitor service status:

```bash
# Check PM2 processes
pm2 status

# Check logs
pm2 logs pm25-collector
pm2 logs pm25-logger

# Check service health
tail -f /var/log/pm25/data_collector.log
```

### Database Maintenance

```bash
# Monitor InfluxDB
influx -execute "SHOW DATABASES"
influx -database pm25gps -execute "SHOW MEASUREMENTS"

# Check data retention
influx -database pm25gps -execute "SELECT COUNT(*) FROM air_quality WHERE time > now() - 24h"
```

### System Resources

Monitor system performance:

```bash
# Check system resources
htop
df -h
free -h

# Monitor network connectivity
ping mqtt.thalay.eu
```

## 🚨 Troubleshooting

### Common Issues

**MQTT Connection Failed:**
- Check network connectivity
- Verify credentials in `.env`
- Check MQTT broker status
- Review firewall settings

**InfluxDB Write Errors:**
- Verify database exists
- Check disk space
- Review InfluxDB logs
- Validate data format

**Arduino Connection Issues:**
- Check WiFi credentials
- Verify MQTT settings
- Check hardware connections
- Monitor serial output

**Frontend Data Issues:**
- Verify GeoJSON file generation
- Check file permissions
- Review web server logs
- Test API endpoints

### Log Locations

- Application logs: `/var/log/pm25/`
- PM2 logs: `~/.pm2/logs/`
- InfluxDB logs: `/var/log/influxdb/`
- Web server logs: `/var/log/apache2/` or `/var/log/nginx/`

## 🔄 Updates and Maintenance

### Updating the System

```bash
# Stop services
pm2 stop all

# Pull updates
git pull origin main

# Install new dependencies
pip install -r requirements.txt

# Update configuration if needed
# Review config/.env.example for new settings

# Restart services
pm2 restart all
```

### Backup Strategy

```bash
# Backup InfluxDB
influxd backup -database pm25gps /backup/influxdb/$(date +%Y%m%d)

# Backup configuration
cp config/.env /backup/config/env-$(date +%Y%m%d)

# Backup web files
tar -czf /backup/web/leaflet-$(date +%Y%m%d).tar.gz /var/www/html
```

## 📊 Performance Tuning

### InfluxDB Optimization

```bash
# Optimize retention policy
influx -database pm25gps -execute "
CREATE RETENTION POLICY thirty_days ON pm25gps
DURATION 30d REPLICATION 1 DEFAULT
"
```

### System Optimization

```bash
# Increase file descriptor limits
echo "* soft nofile 65535" >> /etc/security/limits.conf
echo "* hard nofile 65535" >> /etc/security/limits.conf

# Optimize network settings
echo "net.core.rmem_max = 134217728" >> /etc/sysctl.conf
echo "net.core.wmem_max = 134217728" >> /etc/sysctl.conf
```