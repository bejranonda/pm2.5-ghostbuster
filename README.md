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

## 🛠 Complete System Architecture (v3.0.0)

PM2.5 Ghostbuster is a **unified environmental monitoring system** that transforms a €12 consumer device into a professional-grade mobile air quality monitoring network. The system consists of three integrated components working together as one cohesive solution:

### 🏗 **System Overview: From Device to Global Network**

**The Complete Journey**: IKEA Sensor → Mobile Device → Cloud Network → Global Access

This project demonstrates how community innovation can create sophisticated environmental monitoring infrastructure by connecting:
1. **📱 Mobile Hardware** (PM25ikea/) - Hacked VINDRIKTNING sensors with GPS and WiFi
2. **☁️ Cloud Backend** (Server/) - Professional data processing and storage
3. **🌐 Web Visualization** (Leaflet/) - Real-time global mapping interface

### 🔄 **Unified System Operation**

#### **The Complete Data Lifecycle**
```
🏠 IKEA VINDRIKTNING → 📱 Mobile Enhancement → 🌐 Cloud Processing → 🗺 Global Visualization
        ↓                      ↓                     ↓                    ↓
   €12 Consumer Device → GPS+WiFi Mobile Sensor → MQTT+Database → Real-time Map
```

#### **Single Project Components Working Together**

##### **📱 Mobile PM2.5 Detector (PM25ikea/)**
**The Physical Device**: Transforms IKEA's VINDRIKTNING into a mobile environmental sensor

- **Base Hardware**: IKEA VINDRIKTNING air quality sensor (designed by David Wahl)
- **Enhancement**: ESP8266 microcontroller + GPS module + WiFi connectivity
- **Capability**: Real-time mobile PM2.5 monitoring with location tracking
- **Intelligence**: Offline storage with automatic cloud synchronization
- **Connectivity**: Works with WiFi networks or mobile phone hotspots

##### **☁️ Cloud Infrastructure (Server/)**
**The Data Brain**: Processes and stores all sensor data for global access

- **Data Reception**: MQTT services receive data from all mobile devices
- **Storage**: InfluxDB time-series database for historical data
- **Processing**: Real-time validation, alerts, and data transformation
- **APIs**: REST endpoints for data access and system management
- **Intelligence**: Automated health monitoring and alert systems

##### **🌐 Global Visualization (Leaflet/)**
**The Public Interface**: Makes invisible air pollution visible to everyone

- **Real-time Map**: Live display of PM2.5 data from all mobile devices
- **Historical Data**: 720 hours of air quality history and trends
- **Interactive Features**: Heat maps, clustering, and mobile-responsive design
- **Global Access**: Available worldwide at [map.thalay.eu](https://map.thalay.eu/)

### 🎯 **Single Project Benefits**

This unified approach creates a **complete environmental monitoring ecosystem**:

- **🚶 Individual Impact**: Users can monitor their personal air quality exposure
- **🌆 Community Awareness**: Neighborhoods can track local pollution patterns
- **📊 Scientific Data**: Researchers gain access to distributed monitoring network
- **🌍 Global Understanding**: Worldwide PM2.5 patterns become visible
- **💡 Democratic Technology**: €12 devices democratize environmental monitoring

### 📱 **Mobile PM2.5 Detector Device (`PM25ikea/`)**
**Portable air quality monitoring with intelligent connectivity**

The `PM25ikea/` folder contains the complete implementation for building a **mobile PM2.5 detector device** that revolutionizes environmental monitoring by combining portability, intelligence, and connectivity:

#### **🏗 Device Construction**
- **Base**: Hacked IKEA VINDRIKTNING housing (maintains original compact design)
- **Brain**: ESP8266 D1 Mini WiFi microcontroller
- **Location**: GPS module (Neo-6M) for real-time positioning
- **Power**: USB-C powered with power bank support for mobile operation
- **Status**: LED indicators for connection and operation status

#### **🌐 Intelligent Connectivity Features**

##### **📶 WiFi Management**
- **Automatic Connection**: Connects to known WiFi networks automatically
- **Mobile Hotspot Support**: Users can share internet from their smartphone
- **Captive Portal**: WiFiManager creates setup portal for easy configuration
- **Auto-Recovery**: Automatic reconnection when WiFi becomes available
- **Connection Status**: Visual indicators show connectivity state

##### **☁️ Cloud Data Transmission**
- **Real-time Upload**: Sends PM2.5 + GPS + timestamp to cloud servers
- **MQTT Protocol**: Efficient, lightweight data transmission
- **Leaflet Integration**: Data appears on live web map immediately
- **JSON Format**: Structured data for easy processing and visualization

##### **💾 Offline Intelligence**
```cpp
// Automatic offline storage when no internet
if (!WiFi.connected()) {
    store_locally(pm25_data, gps_location, timestamp);
} else {
    upload_to_cloud(data);
    upload_cached_data(); // Sync stored data
}
```

#### **🚶 Mobile Operation Features**

##### **📍 Movement Detection**
- **Smart Publishing**: Only sends data when device moves significant distance
- **Configurable Threshold**: Minimum movement distance (10-50m) before new reading
- **Battery Optimization**: Reduces data usage and extends operation time
- **Route Tracking**: Creates continuous air quality maps along travel paths

##### **⏱ Real-time Tracking**
```json
{
  "_type": "location",
  "lat": 13.7563,
  "lon": 100.5018,
  "pm25": 35,
  "tst": 1640995200,
  "tid": "AB",
  "t": "v"
}
```

#### **📱 Mobile Usage Scenarios**

##### **🏍 Motorcycle/Vehicle Mounting**
- Mount device on vehicle for route-based monitoring
- Power via vehicle USB or power bank
- Connect to phone hotspot for real-time data
- Monitor pollution exposure during commutes

##### **🚶 Pedestrian Monitoring**
- Portable operation with power bank
- Walking surveys of neighborhood air quality
- Community engagement and awareness activities
- Educational demonstrations

##### **🏢 Temporary Monitoring**
- Quick deployment for specific area monitoring
- Event-based air quality assessment
- Construction site pollution monitoring
- Emergency response applications

#### **🔄 Data Flow Architecture**
```
VINDRIKTNING → ESP8266 → GPS → Local Storage ⟷ WiFi → MQTT → Cloud → Leaflet Map
     ↓             ↓        ↓         ↓           ↓       ↓       ↓         ↓
Internal PM2.5 → Process → Location → Cache → Phone/WiFi → Broker → Server → Real-time Web
```

#### **📊 Offline Resilience**
- **SPIFFS Storage**: Local filesystem for data persistence
- **Automatic Queuing**: Stores readings when offline
- **Smart Sync**: Uploads cached data when connection restored
- **Data Integrity**: No data loss during connectivity interruptions
- **Status Tracking**: LED patterns indicate storage/sync status

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

## 📊 Data Flow & System Integration

### 🔗 **Arduino ↔ Server Connection Architecture**

The PM2.5 Ghostbuster system creates a seamless connection between mobile hardware and cloud infrastructure:

#### **📱 Arduino Device (PM25ikea/) → 🖥 Server Backend (Server/)**

##### **1. Data Collection & Transmission**
```cpp
// Arduino (PM25ikea/picoPM25v4.ino)
void serialize(TinyGPSPlus &gps, char t) {
    DynamicJsonDocument doc(1024);
    doc["_type"] = "location";
    doc["lat"] = gps.location.lat();
    doc["lon"] = gps.location.lng();
    doc["pm25"] = int(state.avgPM25);
    doc["tst"] = now();

    // Publish to MQTT broker
    client.publish(pubtopic, payload, true);
}
```

##### **2. Server Data Reception**
```python
# Server (Server/src/services/mqtt_service.py)
def on_message(client, userdata, msg):
    try:
        data = json.loads(msg.payload.decode())
        air_quality = AirQuality(
            device_id=data.get('tid'),
            pm25=data.get('pm25'),
            latitude=data.get('lat'),
            longitude=data.get('lon'),
            timestamp=data.get('tst')
        )
        influx_service.write_measurement(air_quality)
    except Exception as e:
        logger.error(f"Failed to process message: {e}")
```

#### **🌐 Complete Data Pipeline**

##### **Stage 1: Mobile Device Data Generation**
```
VINDRIKTNING Sensor → ESP8266 Processing → GPS Location → JSON Formatting
```
- **Arduino Scripts**: `PM25gps.ino`, `picoPM25v4.ino`
- **Function**: Read PM2.5 values, combine with GPS coordinates
- **Output**: JSON data packets with location and air quality

##### **Stage 2: Intelligent Transmission**
```
WiFi Connection → MQTT Protocol → Cloud Broker → Server Reception
```
- **Arduino WiFi**: WiFiManager for automatic connection
- **Mobile Hotspot**: Can use smartphone internet connection
- **Offline Storage**: Local caching when no internet available
- **Auto-Sync**: Uploads cached data when connection restored

##### **Stage 3: Server Processing**
```
MQTT Subscriber → Data Validation → Database Storage → Real-time Processing
```
- **Server Scripts**: `main_data_collector.py`, `mqtt_service.py`
- **Database**: InfluxDB time-series storage
- **Processing**: Real-time data validation and transformation

##### **Stage 4: Web Visualization**
```
Database Query → GeoJSON Generation → Leaflet Map → Real-time Display
```
- **Server Scripts**: `geojson_service.py`
- **Web Interface**: `Leaflet/` folder
- **Live Map**: [map.thalay.eu](https://map.thalay.eu/)

### 📡 **MQTT Communication Protocol**

#### **Arduino → Server Communication**
```
Topic Structure:
├── pm25/{deviceID}/air          # Main data stream
├── pm25/{deviceID}/CMND         # Commands to device
└── pm25/{deviceID}/LWT          # Last Will Testament (offline status)

Message Format (JSON):
{
  "_type": "location",
  "lat": 13.7563,          # GPS latitude
  "lon": 100.5018,         # GPS longitude
  "pm25": 35,              # PM2.5 reading (μg/m³)
  "tst": 1640995200,       # Unix timestamp
  "tid": "AB",             # Device ID (last 2 chars)
  "t": "v"                 # Type: 'v'=vehicle, 'p'=ping, 'f'=first_fix
}
```

#### **Server Configuration Matching**
```python
# Server (Server/config/.env)
MQTT_BROKER = "mqtt.thalay.eu"
MQTT_PORT = 1883
MQTT_TOPICS = ["pm25/+/air"]  # Subscribe to all device data

# Arduino (PM25ikea/pico.h)
#define MQTTHOST "mqtt.thalay.eu"
#define BASETOPIC "pm25"
```

### 🔄 **Offline Resilience & Synchronization**

#### **Arduino Offline Handling**
```cpp
// Store data locally when offline
if (!is_online) {
    store(payload);  // Save to SPIFFS filesystem
} else {
    client.publish(pubtopic, payload, true);
    unload_store();  // Upload any cached data
}
```

#### **Server Robust Reception**
```python
# Handle intermittent connections gracefully
def on_disconnect(client, userdata, rc):
    logger.warning("MQTT disconnected, attempting reconnection...")
    reconnect_with_backoff()

def on_connect(client, userdata, flags, rc):
    logger.info("MQTT connected, subscribing to topics...")
    client.subscribe("pm25/+/air")
```

### 📱 **Mobile Device → Cloud Workflow**

#### **Real-time Operation**
1. **Arduino reads** PM2.5 from VINDRIKTNING sensor via UART
2. **GPS module provides** location coordinates
3. **ESP8266 combines** data into JSON format
4. **WiFi transmits** to MQTT broker (direct or via mobile hotspot)
5. **Server receives** and validates data
6. **InfluxDB stores** time-series measurements
7. **Leaflet map updates** in real-time

#### **Offline Operation**
1. **Arduino continues** collecting PM2.5 and GPS data
2. **Local storage** saves data to SPIFFS filesystem
3. **WiFi monitoring** checks for connection restoration
4. **Automatic sync** uploads all cached measurements
5. **Server processing** handles bulk data upload
6. **Map visualization** shows complete route history

### 🎯 **System Integration Benefits**

- **🔄 Seamless Operation**: Mobile device works online or offline
- **📱 User Flexibility**: Can use phone hotspot for immediate results
- **☁️ Cloud Integration**: Real-time data appears on live web map
- **💾 Data Persistence**: No measurements lost during connectivity issues
- **🌍 Community Access**: Data available globally via web interface
- **📊 Historical Analysis**: Complete route and temporal data preservation

This architecture enables users to carry the mobile PM2.5 detector anywhere, see real-time results on their phone, and contribute to a global environmental monitoring network.

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
├── PM25ikea/                       # 📱 Mobile PM2.5 Detector (2023)
│   ├── README_PM25ikea.md         # Complete mobile device documentation
│   ├── PM25ikea/                  # Mobile device Arduino implementations
│   │   ├── PM25gps/               # 🚗 Vehicle/mobile GPS+PM2.5 monitoring
│   │   │   └── PM25gps.ino        # Original mobile detector script
│   │   ├── picoPM25v4/            # 📱 Advanced mobile version with WiFiManager
│   │   │   └── picoPM25v4.ino     # Enhanced mobile sensor implementation
│   │   ├── picoPM25b*/            # 🔬 Development versions for mobile device
│   │   └── PicoPM25/              # 📍 Location-based sensor prototypes
│   ├── GPS/                       # 🗺 GPS module implementations for mobility
│   ├── AMG8833-Thermal-Camera/    # 🌡 Thermal imaging integration
│   ├── CO2/                       # 💨 CO2 sensor modules
│   ├── LEDws2812b/                # 💡 LED status indicators
│   └── Library/                   # 📚 Arduino libraries and dependencies
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

### 📚 **Project Evolution: Single Vision, Multiple Phases**

PM2.5 Ghostbuster represents a **single, evolving project** that has grown from prototype to professional system while maintaining its core vision of democratizing environmental monitoring:

#### **2023: Innovation Foundation (PM25ikea/)**
**Original Mobile Device Implementation**

The project began with the innovative concept of transforming IKEA's VINDRIKTNING into a mobile environmental monitoring device:

- **Core Innovation**: Direct UART access to consumer device internals
- **Mobile Capability**: GPS tracking + WiFi connectivity for real-time mapping
- **Offline Intelligence**: Local data storage with automatic cloud synchronization
- **User Accessibility**: Mobile hotspot support for immediate real-time tracking
- **Community Focus**: Affordable technology for grassroots environmental monitoring

**Key Implementation Scripts:**
- `PM25gps.ino` - Original mobile GPS+PM2.5 monitoring
- `picoPM25v4.ino` - Advanced WiFiManager integration
- Complete Arduino library ecosystem for mobile sensors

#### **2025: Professional Evolution (Arduino/ + Server/ + Leaflet/)**
**Enterprise-Grade System Architecture**

The same core vision expanded into a complete environmental monitoring platform:

- **Enhanced Mobile Device**: Professional-grade Arduino implementation
- **Cloud Infrastructure**: Enterprise server architecture with APIs and alerts
- **Global Visualization**: Real-time web mapping for worldwide access
- **Scalable Network**: Support for multiple devices and communities
- **Professional Standards**: Security, reliability, and operational excellence

#### **2025: Complete Integration (v3.0.0)**
**Unified Project Documentation**

The final phase brings together the complete project story:

- **Historical Preservation**: Original innovation techniques documented
- **Educational Value**: Complete learning resource for IoT sensor hacking
- **Community Legacy**: Full documentation of collaborative art-science success
- **Technical Evolution**: Clear progression from prototype to production
- **Global Impact**: Demonstration of community-driven technology scaling

### 🔗 **Single Project Architecture: All Components Connected**

This is **one integrated system** where each component serves the unified vision of making invisible air pollution visible through accessible technology:

#### **🔄 Complete System Workflow**

##### **User Journey: From Device to Global Impact**
1. **🔨 Build Mobile Device** (PM25ikea/) - Hack IKEA VINDRIKTNING sensor
2. **📱 Carry & Monitor** - Mobile PM2.5 detection with phone hotspot
3. **☁️ Automatic Upload** - Data flows to cloud infrastructure
4. **🌍 Global Visibility** - Measurements appear on live world map
5. **🏘 Community Impact** - Local patterns become visible to everyone

##### **Technical Integration Flow**
```
📱 Mobile Device (PM25ikea/)
    ↓ WiFi/MQTT
☁️ Cloud Backend (Server/)
    ↓ Real-time Processing
🌐 Global Map (Leaflet/)
    ↓ Public Access
🌍 Community Awareness & Action
```

#### **🎯 Unified Project Vision**

**Single Goal**: Transform €12 consumer devices into a global environmental monitoring network

**Three Integrated Components**:
- **Hardware Innovation** (PM25ikea/) - Mobile sensor technology
- **Infrastructure Intelligence** (Server/) - Cloud data processing
- **Community Access** (Leaflet/) - Global visualization platform

**One Complete Solution**: Users build mobile devices, carry them anywhere, and contribute to global environmental awareness through real-time data sharing.

#### **📊 System Integration Examples**

##### **Real-world Usage Scenario**
```
🏍 User mounts mobile device on motorcycle
📱 Connects device to phone hotspot
🚗 Rides through different neighborhoods
☁️ PM2.5 data automatically uploads to cloud
🗺 Route appears on live map in real-time
🏘 Community sees pollution patterns
📈 Data informs local environmental decisions
```

##### **Technical Coordination**
```cpp
// Mobile Device (PM25ikea/picoPM25v4.ino)
serialize(gps, 'v');  // Format data for transmission
client.publish(pubtopic, payload, true);  // Send to cloud

// Server (Server/src/services/mqtt_service.py)
influx_service.write_measurement(air_quality);  // Store data

// Web Interface (Leaflet/js/)
updateMap(realTimeData);  // Display on global map
```

This **single, integrated project** demonstrates how community innovation can create sophisticated environmental monitoring infrastructure that serves individual users while building collective environmental awareness.

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