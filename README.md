# PM2.5 Ghostbuster

> *"PM2.5 is like ghost. It exists even if it is invisible."*

A collaborative art-science project making invisible air pollution visible through DIY sensors, real-time data visualization, and community engagement.

## 🌍 About

PM2.5 Ghostbuster is a joint initiative between **Werapol Bejranonda** (Engineer - Germany) and **Baan Noorg Collaborative Arts and Culture** (Artist Collective - Thailand) focused on raising awareness about PM2.5 air pollution in Rajabura province, Thailand.

The project bridges the gap between hard science and public understanding by using art and cultural approaches to make environmental data accessible to local communities.

**Live Data**: [map.thalay.eu](https://map.thalay.eu/)

## 🎯 Project Philosophy

This project transforms complex environmental science into understandable, actionable information for civilians. By treating PM2.5 pollution as an invisible "ghost" that needs to be detected and understood, we empower communities to monitor and respond to their air quality.

## 🛠 Technical Architecture (v2.0.0 - Refactored)

### 🔧 Arduino Sensor Device (`Arduino/`)
**Modular ESP8266-based sensor with enhanced reliability**
- **Main Sketch**: `pm25_ghostbuster.ino` - Refactored main application
- **Modules**:
  - `wifi_manager.*` - WiFi connection management with auto-recovery
  - `mqtt_client.*` - MQTT publishing with guaranteed delivery
  - `config.h.example` - Secure configuration template
- **Features**:
  - 🛡 **Secure Configuration**: Credentials separated from code
  - 🔄 **Auto-Recovery**: Automatic WiFi and MQTT reconnection
  - 📍 **Smart GPS**: Location-based publishing with distance thresholds
  - 🚨 **Status Indicators**: LED patterns show connection status
  - ⚡ **Power Efficient**: Optimized for battery operation

### 🖥 Server Backend (`Server/`)
**Enterprise-grade Python services with proper error handling**

#### Configuration (`config/`)
- `settings.py` - Centralized configuration management
- `.env.example` - Secure environment variable template
- Environment-based configuration with validation

#### Services (`src/services/`)
- `mqtt_service.py` - MQTT client with connection management
- `influx_service.py` - InfluxDB operations with batch processing
- `geojson_service.py` - GeoJSON generation and caching

#### Models (`src/models/`)
- `air_quality.py` - Data models with validation and transformation

#### Applications
- `main_data_collector.py` - Main data processing service (replaces PM25ikeav2.py)
- `main_mqtt_logger.py` - MQTT debugging service (replaces PM25mqttLog.py)

#### Features
- 🔒 **Security First**: No hard-coded credentials, input validation
- 🏗 **Modular Design**: Separation of concerns, testable components
- 📊 **Robust Logging**: Structured logging with timezone awareness
- 🔄 **Error Recovery**: Graceful handling of network and database issues
- ⚡ **Performance**: Batch processing and connection pooling

### 🗺 Web Visualization (`Leaflet/`)
- Interactive web mapping interface built with Leaflet.js
- **Enhanced Features**:
  - Real-time PM2.5 data visualization
  - Heat map overlays with color coding
  - Marker clustering for dense data
  - Historical data viewing (up to 720 hours)
  - Mobile-responsive design
  - Full-screen mapping capability
  - GPS location services
  - Google Analytics integration

## 📊 Data Flow

1. **Collection**: Arduino devices measure PM2.5 levels and GPS coordinates
2. **Transmission**: Data sent via MQTT to cloud server
3. **Storage**: InfluxDB stores time-series air quality data
4. **Processing**: Python scripts generate GeoJSON files every minute
5. **Visualization**: Web interface displays real-time and historical data

## 🚀 Getting Started (v2.0.0)

### Prerequisites
- ESP8266 development board (NodeMCU, Wemos D1 Mini)
- PM2.5 sensor module
- GPS module (Neo-6M recommended)
- InfluxDB 1.x database
- MQTT broker (Mosquitto recommended)
- Python 3.8+
- Web server (Apache/Nginx)

### Quick Start

#### 1. Arduino Sensor Setup
```bash
# Copy configuration template
cp Arduino/config.h.example Arduino/config.h

# Edit config.h with your WiFi and MQTT credentials
# Upload pm25_ghostbuster.ino to ESP8266
```

#### 2. Server Setup
```bash
# Install Python dependencies
cd Server
pip install -r requirements.txt

# Configure environment
cp config/.env.example config/.env
# Edit .env with your database and MQTT settings

# Start services with PM2
pm2 start src/main_data_collector.py --name pm25-collector --interpreter python3
pm2 start src/main_mqtt_logger.py --name pm25-logger --interpreter python3
```

#### 3. Web Interface
```bash
# Deploy to web server
cp -r Leaflet/* /var/www/html/
```

📖 **For detailed deployment instructions, see [DEPLOYMENT.md](DEPLOYMENT.md)**

## 📁 Project Structure (v2.1.0)

```
PM25-Ghostbuster/
├── Arduino/                        # 🔧 Sensor Firmware
│   ├── pm25_ghostbuster.ino       # Main sketch (refactored)
│   ├── config.h.example           # Configuration template
│   ├── wifi_manager.*             # WiFi management module
│   ├── mqtt_client.*              # MQTT client module
│   └── pico.h.example             # Legacy config (deprecated)
├── Server/                         # 🖥 Backend Services (Enhanced)
│   ├── config/                    # Configuration management
│   │   ├── settings.py            # Settings loader
│   │   └── .env.example           # Environment template (v2.1.0)
│   ├── src/                       # Source code
│   │   ├── services/              # Business logic
│   │   │   ├── mqtt_service.py    # MQTT handling
│   │   │   ├── influx_service.py  # Database operations
│   │   │   ├── geojson_service.py # Data export
│   │   │   ├── alert_service.py   # 🆕 Alert system (v2.1.0)
│   │   │   └── api_service.py     # 🆕 REST API (v2.1.0)
│   │   ├── models/                # Data models
│   │   │   └── air_quality.py     # Measurement model
│   │   ├── utils/                 # Utilities
│   │   │   ├── logger.py          # Logging system
│   │   │   └── timezone_utils.py  # Timezone handling
│   │   ├── main_data_collector.py # Enhanced main service (v2.1.0)
│   │   └── main_mqtt_logger.py    # Debug service
│   ├── scripts/                   # 🆕 Automation Scripts (v2.1.0)
│   │   ├── setup.sh              # Automated setup script
│   │   ├── health_check.py       # System health monitoring
│   │   └── run_api.py            # Standalone API server
│   ├── ecosystem.config.js        # 🆕 PM2 configuration (v2.1.0)
│   ├── requirements.txt           # Python dependencies (enhanced)
│   └── pm25/                      # Legacy files (deprecated)
├── Leaflet/                       # 🗺 Web Interface
│   ├── index.html                # Main web interface
│   ├── css/                      # Stylesheets
│   ├── js/                       # JavaScript libraries
│   └── images/                   # Images and assets
├── DEPLOYMENT.md                  # 📖 Deployment guide
└── CHANGELOG.md                   # 📋 Version history
```

### 🆕 **New in v2.1.0**
- **Alert Service**: Intelligent PM2.5 threshold monitoring with WHO/EPA standards
- **REST API**: Complete HTTP API for data access and system management
- **Automation Scripts**: One-command setup and comprehensive health monitoring
- **PM2 Integration**: Professional process management and deployment
- **Enhanced Configuration**: Extended settings for alerts, API, and monitoring

## 🚀 What's New in v2.1.0

### 🚨 **Advanced Alert System**
- **Smart Thresholds**: WHO/EPA-based PM2.5 alert levels (Good, Moderate, Unhealthy, etc.)
- **Real-time Notifications**: Instant alerts when pollution levels exceed thresholds
- **Email Notifications**: Configurable SMTP-based alert delivery
- **Alert History**: Track and analyze alert patterns over time
- **Acknowledgment System**: Manual alert acknowledgment and management

### 🌐 **REST API Integration**
- **Real-time Data Access**: RESTful API for all system data
- **Health Monitoring**: Comprehensive system health checks via API
- **Data Export**: JSON/CSV export functionality for analysis
- **Device Management**: API endpoints for device statistics and monitoring
- **CORS Support**: Web-friendly API for frontend integration

### 📊 **Enhanced Monitoring & Statistics**
- **System Metrics**: Real-time performance and uptime tracking
- **Resource Monitoring**: CPU, memory, and disk space monitoring
- **Connection Health**: MQTT, InfluxDB, and API health checks
- **Statistical Logging**: Automated statistics with 5-minute intervals
- **Service Management**: PM2 ecosystem with process monitoring

### 🔧 **Automation & DevOps**
- **Setup Automation**: One-command installation script
- **Health Checks**: Comprehensive system health monitoring
- **PM2 Integration**: Professional process management configuration
- **Service Scripts**: Standalone API server and utility scripts
- **Docker Ready**: Container-friendly configuration

### 🎯 **Enhanced by Claude Code Assistant**
- **AI-Powered Development**: Advanced features designed by Claude
- **Clean Architecture**: Professional-grade code organization
- **Comprehensive Documentation**: Detailed guides and examples
- **Best Practices**: Security, performance, and maintainability focus

## ⚡ What's New in v2.0.0

### 🔒 **Security & Reliability**
- **Eliminated Hard-coded Credentials**: All sensitive data moved to secure environment files
- **Input Validation**: Robust data validation and sanitization
- **Error Recovery**: Automatic reconnection and graceful error handling
- **Secure Configuration**: Template-based config with credential separation

### 🏗 **Architecture Improvements**
- **Modular Design**: Separation of concerns with dedicated services
- **Professional Logging**: Structured logging with timezone awareness
- **Performance Optimizations**: Batch processing and connection pooling
- **Health Monitoring**: Built-in health checks and monitoring capabilities

### 🛠 **Developer Experience**
- **Clean Code Structure**: Well-organized, maintainable, and testable code
- **Comprehensive Documentation**: Detailed deployment and setup guides
- **Environment Management**: Easy configuration with `.env` files
- **Dependency Management**: Proper requirements and version control

### 📊 **Operational Excellence**
- **Process Management**: PM2-based service management
- **Log Management**: Centralized logging with rotation
- **Database Optimization**: Improved InfluxDB queries and retention policies
- **Monitoring**: Built-in health checks and performance metrics

## 🎨 Art-Science Integration

This project demonstrates how technology can be made accessible through creative approaches:
- **Metaphor**: Treating PM2.5 as invisible "ghosts" that need detection
- **Community Focus**: Empowering locals with their own monitoring capabilities
- **Visual Storytelling**: Interactive maps tell the story of air quality over time
- **Cultural Bridge**: Connecting scientific data with cultural understanding
- **Technical Accessibility**: Clean, documented code enables community contributions

## 🌱 Community Impact

- **Education**: Helps communities understand air pollution patterns
- **Awareness**: Makes invisible environmental threats visible
- **Empowerment**: Provides tools for community-led monitoring
- **Action**: Enables data-driven environmental advocacy

## 🤝 Collaboration

This project represents a successful collaboration between:
- **Technical Expertise** (Werapol Bejranonda, Germany)
- **Local Cultural Knowledge** (Baan Noorg Collaborative Arts and Culture, Thailand)
- **Community Engagement** (Rajabura province residents)

## 📈 Data Specifications

- **Measurement Range**: PM2.5 particulate matter (μg/m³)
- **Update Frequency**: Real-time (configurable intervals)
- **Data Retention**: 720 hours (30 days) of historical data
- **Geographic Coverage**: GPS-tracked mobile measurements
- **Time Zone**: Bangkok Time (UTC+7)

## 🔧 Configuration

Key configuration parameters:
- **MQTT Topics**: `pm25/{device_id}/air` for sensor data
- **Database**: InfluxDB measurement `air_quality`
- **Update Interval**: 60 seconds for GeoJSON generation
- **Data Retention**: 720 hours configurable in `PM25ikeav2.py`

## 📄 License

This project is open source and available for community use and adaptation.

## 🙏 Acknowledgments

### Core Collaboration Team
- **Werapol Bejranonda** - Thai Engineer/Project Lead (based in Kassel, Germany)
- **Baan Noorg Collaborative Arts and Culture** - Host Organization/Collaborative Partner (Nong Pho, Thailand)

### Key Collaborators
- **Chiara Giardi** - Curator, Interpreter, Continuator
- **Nisa Jewcharoen** - Coordinator

### Baan Noorg Team Members and Local Participants in Nong Pho
- **JiandYin**
- **Ploy**
- **Im**
- **Boss**

### Community
- **Local Communities** in Rajabura province for participation and testing

---

*Making the invisible visible, one sensor at a time.* 👻