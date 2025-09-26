# PM2.5 Ghostbuster

> *"PM2.5 is like ghost. It exists even if it is invisible."*

A collaborative art-science project making invisible air pollution visible through IKEA-hacked sensors, real-time data visualization, and community engagement.

**Built on IKEA VINDRIKTNING** - Transforming consumer air quality sensors into a powerful environmental monitoring network.

**Complete Development Journey**: From prototype hacking in 2023 to enterprise-grade monitoring system in 2025, showcasing the evolution of community-driven environmental technology.

## 🌍 About

PM2.5 Ghostbuster is a joint initiative between **Werapol Bejranonda** (Engineer - Germany) and **Baan Noorg Collaborative Arts and Culture** (Artist Collective - Thailand) focused on raising awareness about PM2.5 air pollution in Rajabura province, Thailand.

The project bridges the gap between hard science and public understanding by using art and cultural approaches to make environmental data accessible to local communities.

**Live Data**: [map.thalay.eu](https://map.thalay.eu/)

## 🎯 Project Philosophy

This project transforms complex environmental science into understandable, actionable information for civilians. By treating PM2.5 pollution as an invisible "ghost" that needs to be detected and understood, we empower communities to monitor and respond to their air quality.

### 🕰 **Development Timeline & Evolution**

**2023**: **Pioneer Phase** - Original VINDRIKTNING hacking and mobile sensor prototypes
**2025**: **Professional Phase** - Enterprise-grade architecture with AI-assisted development
**2025**: **Integration Phase** - Complete historical documentation and unified codebase

This timeline represents the natural evolution from grassroots innovation to professional environmental monitoring infrastructure, demonstrating how community-driven projects can scale into significant technological contributions.

## 🛠 Technical Architecture (v3.0.0 - Complete Historical Integration)

### 🏭 **IKEA VINDRIKTNING Foundation**
**Hacked consumer air quality sensor transformed into IoT monitoring device**

The project ingeniously repurposes IKEA's VINDRIKTNING air quality sensor, designed by David Wahl, which originally provides:
- **PM2.5 Detection**: Particle monitoring for indoor air quality
- **Visual Indicators**: Green (good), Yellow (ok), Red (not good) air quality levels
- **Consumer Friendly**: USB-C powered, small form factor for any room
- **Professional Accuracy**: Suitable for both small and large spaces

**Our Enhancement**: By hacking the internal UART communication and integrating ESP8266 microcontroller, GPS module, and WiFi connectivity, we transform this €12 consumer device into a powerful mobile IoT sensor capable of:
- Real-time GPS-tracked air quality monitoring
- MQTT data transmission to cloud servers
- WiFi connectivity with automatic reconnection
- Location-based data publishing with movement detection
- Integration with professional environmental monitoring networks

**Technical Implementation**: The hack involves accessing the PM2.5 sensor's internal serial communication (UART) to read raw particle measurements, then transmitting this data along with GPS coordinates via WiFi to MQTT brokers for real-time environmental monitoring and mapping.

### 🔧 **Enhanced Arduino Sensor Device (`Arduino/`)**
**Modular ESP8266-based enhancement with professional reliability**
- **Hardware Base**: IKEA VINDRIKTNING PM2.5 sensor (hacked for data access)
- **Main Sketch**: `pm25_ghostbuster.ino` - Professional IoT implementation
- **Modules**:
  - `wifi_manager.*` - WiFi connection management with auto-recovery
  - `mqtt_client.*` - MQTT publishing with guaranteed delivery
  - `config.h.example` - Secure configuration template
- **Features**:
  - 🛡 **Secure Configuration**: Credentials separated from code
  - 🔄 **Auto-Recovery**: Automatic WiFi and MQTT reconnection
  - 📍 **Smart GPS**: Location-based publishing with distance thresholds
  - 🚨 **Status Indicators**: LED patterns show connection status
  - ⚡ **Power Efficient**: Optimized for continuous operation
  - 🏭 **Consumer Hardware**: Built on proven IKEA sensor technology

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

## 🚀 Getting Started (v3.0.0)

### Hardware Requirements

#### IKEA VINDRIKTNING Air Quality Sensor
**The heart of PM2.5 Ghostbuster** - Professional PM2.5 detection at consumer prices

- **Product**: IKEA VINDRIKTNING Air Quality Sensor (Product ID: 804.982.46)
- **Designer**: David Wahl
- **Original Purpose**: Consumer air quality monitoring for homes
- **Availability**: Available at IKEA stores globally
  - ✅ **Available**: [India IKEA](https://www.ikea.com/in/en/p/vindriktning-air-quality-sensor-80498246/)
  - ⚠️ **Limited**: Germany IKEA (discontinued - check online marketplaces)
  - 🔍 **Alternative sources**: eBay, Amazon, local electronics stores
- **Original Features**:
  - Professional PM2.5 particle detection
  - Visual indicators: Green (good), Yellow (ok), Red (not good)
  - USB-C powered (cable and adapter included)
  - Compact design (6×15×6 cm) suitable for any room
  - Works effectively in both small and large spaces
  - Designed to work with IKEA FÖRNUFTIG air purifier

#### **The Hacking Process** 🔧
**Transforming Consumer Device into Professional IoT Sensor**

This project involves a sophisticated hardware hack of the VINDRIKTNING sensor to access its internal data stream:

1. **Hardware Modification**: Opening the VINDRIKTNING housing and accessing internal components
2. **UART Access**: Tapping into the sensor's UART communication protocol to read raw PM2.5 data
3. **Microcontroller Integration**: Installing ESP8266 D1 Mini inside the original housing
4. **GPS Module Addition**: Adding Neo-6M GPS module for location tracking
5. **WiFi Connectivity**: Enabling wireless data transmission via built-in ESP8266 WiFi
6. **Power Management**: Utilizing the original USB-C power supply (5V) with power bank support

**Original Development (2023)**: The hacking concept was initially developed using Arduino scripts that combined:
- PM2.5 sensor data reading via UART
- GPS location tracking with movement detection
- WiFi connectivity with automatic reconnection
- MQTT publishing for real-time data transmission
- Local data storage with offline caching capabilities

#### Additional Components
- **ESP8266 development board** (NodeMCU, Wemos D1 Mini)
- **GPS module** (Neo-6M recommended for location tracking)
- **Jumper wires and breadboard** (for sensor connections)

#### Software & Infrastructure
- **InfluxDB 1.x** database
- **MQTT broker** (Mosquitto recommended)
- **Python 3.8+**
- **Arduino IDE** with ESP8266 board package
- **Web server** (Apache/Nginx for frontend)

> **💡 Why VINDRIKTNING?** This ingenious hack transforms an affordable €12 consumer air quality sensor into a professional IoT monitoring device, democratizing environmental monitoring for communities worldwide.

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

📖 **For detailed information, see:**
- [DEPLOYMENT.md](DEPLOYMENT.md) - Complete deployment guide
- [API.md](API.md) - REST API documentation
- [CONFIGURATION.md](CONFIGURATION.md) - Configuration guide

## 📁 Project Structure (v3.0.0)

```
PM25-Ghostbuster/
├── Arduino/                        # 🔧 Sensor Firmware (Current)
│   ├── pm25_ghostbuster.ino       # Main sketch (refactored)
│   ├── config.h.example           # Configuration template
│   ├── wifi_manager.*             # WiFi management module
│   ├── mqtt_client.*              # MQTT client module
│   └── pico.h.example             # Legacy config (deprecated)
├── PM25ikea/                       # 🏗 Original Implementation (2023)
│   ├── PM25ikea/                  # Original Arduino scripts
│   │   ├── PM25gps/               # GPS-enabled PM2.5 monitoring
│   │   │   └── PM25gps.ino        # Main GPS+PM2.5 script
│   │   ├── picoPM25v4/            # Advanced version with WiFiManager
│   │   │   └── picoPM25v4.ino     # Enhanced sensor implementation
│   │   └── picoPM25b*/            # Development versions
│   ├── GPS/                       # GPS module implementations
│   ├── AMG8833-Thermal-Camera/    # Thermal camera integration
│   ├── CO2/                       # CO2 sensor modules
│   ├── LEDws2812b/                # LED control systems
│   └── Library/                   # Arduino libraries and dependencies
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
├── API.md                         # 🌐 REST API documentation
├── CONFIGURATION.md               # ⚙️ Configuration guide
├── DEPLOYMENT.md                  # 📖 Deployment guide
└── CHANGELOG.md                   # 📋 Version history
```

### 🆕 **New in v3.0.0 - Historical Integration & Complete Documentation**
- **Complete Historical Integration**: Full integration of original 2023 PM25ikea implementation
- **Development Timeline Documentation**: Comprehensive documentation of project evolution
- **Legacy Script Preservation**: Complete original Arduino scripts with detailed documentation
- **Technical Heritage**: Documentation of innovative VINDRIKTNING hacking techniques
- **Project Story Completion**: Unified narrative from prototype to enterprise system
- **Educational Resources**: Historical implementations for learning and research
- **Community Impact Documentation**: Complete story of collaborative art-science project

### 🎯 **Previous Major Releases**

#### **v2.1.0 Features (2025)**
- **Alert Service**: Intelligent PM2.5 threshold monitoring with WHO/EPA standards
- **REST API**: Complete HTTP API for data access and system management
- **Automation Scripts**: One-command setup and comprehensive health monitoring
- **PM2 Integration**: Professional process management and deployment
- **Enhanced Configuration**: Extended settings for alerts, API, and monitoring

### 🏗 **Legacy PM25ikea Implementation (2023)**
**Original VINDRIKTNING hacking and mobile sensor development**

The `PM25ikea/` folder contains the original implementation that pioneered the VINDRIKTNING hacking concept:

#### **Key Original Scripts:**
- **`PM25gps.ino`**: Original GPS+PM2.5 monitoring with OwnTracks-compatible JSON publishing
- **`picoPM25v4.ino`**: Advanced implementation with WiFiManager, MQTT, and automated reconnection
- **GPS Integration**: Real-time location tracking with movement-based publishing
- **Mobile Sensor Network**: Battery-powered operation for mobile air quality mapping

#### **Technical Innovations (2023):**
- **UART Communication**: Direct access to VINDRIKTNING internal sensor data
- **Dual Serial Setup**: GPS (D1/D2) and PM2.5 sensor (D4) on separate UART channels
- **Smart Publishing**: Distance-based triggers (minimum 10-50m movement) for efficient data usage
- **Offline Storage**: SPIFFS filesystem for data caching during connectivity loss
- **OwnTracks Protocol**: GPS tracking compatibility with existing location tracking systems
- **WiFiManager Integration**: Captive portal configuration for easy sensor deployment

#### **Historical Context:**
This original implementation demonstrated the feasibility of transforming a €12 consumer air quality sensor into a professional-grade mobile environmental monitoring device. The scripts show early innovations in IoT sensor hacking and established the foundation for the current enterprise-grade v2.x architecture.

## 🎉 **What's New in v3.0.0 - Complete Project Integration**

### 📚 **Historical Code Integration**
**Preserving Innovation Heritage**: For the first time, the complete original 2023 codebase has been integrated into the main repository, providing:

- **`PM25ikea/` Folder**: Complete original Arduino implementations from 2023
- **Technical Documentation**: Detailed analysis of pioneering VINDRIKTNING hacking techniques
- **Evolution Story**: Clear documentation showing the progression from prototype to enterprise system
- **Educational Value**: Historical code serves as learning resource for IoT development and sensor hacking

### 🔗 **Unified Development Narrative**
**From Hack to Enterprise**: v3.0.0 completes the story by showing the complete journey:

**2023 Innovation** → **2025 Professional** → **2025 Integration**
- Original hacking concepts → Enterprise architecture → Complete documentation
- Prototype scripts → Production systems → Historical preservation
- Community proof-of-concept → Professional deployment → Educational resource

### 🏛 **Technical Heritage Documentation**
**Preserving Pioneering Techniques**: The original PM25ikea scripts documented innovative approaches:

- **UART Communication**: Direct access to consumer device internals
- **Dual Serial Management**: Simultaneous GPS and sensor data handling
- **Movement-based Publishing**: Intelligent data transmission optimization
- **Offline Storage**: Resilient data handling during connectivity issues
- **OwnTracks Integration**: Professional GPS tracking protocol implementation

### 🌍 **Community Impact Recognition**
**Art-Science Collaboration Success**: v3.0.0 fully documents the collaborative achievement:

- **Thai-German Partnership**: International cooperation in environmental technology
- **Community Empowerment**: Transforming €12 devices into professional monitoring tools
- **Cultural Bridge**: Making environmental science accessible through art and technology
- **Open Source Impact**: Complete codebase available for global community replication

### 📖 **Documentation Excellence**
**Complete Project Story**: Comprehensive documentation now includes:

- **Technical Evolution**: From prototype scripts to enterprise architecture
- **Hardware Hacking Guide**: Detailed VINDRIKTNING modification process
- **Development Timeline**: Clear progression through different phases
- **Legacy Value**: Preserving original innovation for future generations

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

### 🎯 **Professional Development**
- **Advanced Features**: Enterprise-grade monitoring and alerting
- **Clean Architecture**: Professional code organization and patterns
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
- **Technical Excellence**: Clean, documented code enables community contributions

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

### AI Development Support
This project has evolved through different phases of AI-assisted development:

#### **Phase 1: Original Development (2023)**
The initial scripts and core functionality were **composed using suggestions from ChatGPT and Google Gemini**:
- Core data collection and MQTT communication scripts
- Basic InfluxDB integration and data processing
- Initial web visualization interface
- Arduino sensor firmware foundation

#### **Phase 2: Professional Refactoring (2025)**
**Two years later**, the entire system was **comprehensively refactored by Claude (Anthropic)**:
- Complete architectural redesign with modular structure
- Security improvements and credential management
- Advanced alert system with WHO/EPA standards
- Professional REST API development
- Automation tools and deployment scripts
- Enhanced documentation and best practices implementation

This development timeline demonstrates the evolution from AI-suggested prototype scripts to a production-ready, professionally architected system through iterative AI assistance spanning multiple years.

---

*Making the invisible visible, one sensor at a time.* 👻