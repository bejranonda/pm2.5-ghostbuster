# PM2.5 Ghostbuster

> *"PM2.5 is like ghost. It exists even if it is invisible."*

*Making invisible air pollution visible through hacked IKEA sensors and community-powered data*

**PM2.5 Ghostbuster** transforms €12 IKEA VINDRIKTNING air quality sensors into GPS-enabled mobile pollution detectors that reveal invisible environmental threats.

Like ghost-hunting equipment for microscopic particles, this open-source IoT system combines hardware hacking, real-time mapping, and community science—empowering anyone to track air quality as they move through neighborhoods, identify pollution hotspots, and make atmospheric data tangible. Born from a 2023 artist residency in rural Thailand, the project has generated 12 months of citizen-collected environmental data across four provinces.

![Mobile PM2.5 Detector Device in service](https://github.com/user-attachments/assets/4195d37e-1199-4a4c-8236-67b77a9f35be)
 
 *A transformed IKEA air quality sensor becomes a mobile environmental monitoring device*


## 📍 Exhibition Context

- **Project Name:** PM2.5 Ghostbuster
- **Medium:** Interactive Environmental Monitoring Installation / Community Intervention
- **Exhibition/Activity:** Artist-in-Residence Program & Community Workshop
- **Development Period:** May 24-28, 2023 (Artist Residency) | Active Monitoring: May 2023 - April 2024
- **Venue:** Baan Noorg Collaborative Arts & Culture, Nong Pho, Ratchaburi Province, Thailand
- **Type:** Workshop / Public Intervention / Online Platform
- **Geographic Coverage:** Ratchaburi, Kanchanaburi, Bangkok, Samut Prakan (Thailand), Kassel (Germany)
- **Web-App:** [map.thalay.eu](https://map.thalay.eu/) (system archived due to funding constraints)

This collaborative art-science project emerged from a five-day artist residency in rural Thailand, where community members built mobile air quality sensors together. The monitoring network operated for six months across four provinces, making invisible air pollution visible to local communities before being archived in November 2023.


## 🔍 The Experience: Revealing Invisible Threats

**Imagine carrying a device** no bigger than a smartphone that reveals the invisible—microscopic particles floating in the air around you, potentially harming your health with every breath.

**PM2.5 Ghostbuster transforms** affordable €12 IKEA air quality sensors into mobile environmental detectives that track pollution as you move through your city, your neighborhood, your daily commute.

### How People Used It

**🏍️ On the Move**
Mount the sensor on your motorcycle or carry it while walking. The device continuously measures air quality and plots data on a live map accessible to anyone with internet.

**🗺️ Seeing the Invisible**
Streets glow red with dangerous pollution levels. Neighborhoods breathe clean and green. Patterns emerge showing how pollution moves through communities.

**📶 Works Anywhere—Internet or Not**
Connect the device to your phone's hotspot → data appears on the global map within seconds. No internet? The device remembers everything locally and syncs automatically when connection returns.

**Perfect for mobile vendors, ice cream bike-shops, or motorcycle commuters:** Collect PM2.5 measurements, timestamps, and GPS coordinates throughout the day while offline. When you return home or pass a public WiFi hotspot, all accumulated data automatically transfers to the server and appears on the public map—revealing pollution patterns across your community without requiring constant connectivity.


![349018209_946426160008440_2158539357010701521_n](https://github.com/user-attachments/assets/802d6cce-bbd2-4526-b83a-f1f857d2e3d6)

*Bringing the Mobile PM2.5 Detector to the Local Community*


### What Made It Powerful

**💰 Accessible Technology** — €12 IKEA VINDRIKTNING sensor (designed by David Wahl) + GPS tracking + WiFi connectivity

**🌍 Environmental Democracy** — Professional-grade air quality tracking in the hands of anyone who wants to understand the air they breathe

**📊 Community Data Network** — Twelve months of active monitoring across Ratchaburi, Kanchanaburi, Bangkok, and Samut Prakan provinces generated rich environmental data that official monitoring stations couldn't provide. Each mobile device acts as a moving sensor, collectively mapping pollution hotspots across entire communities at a fraction of the cost of fixed monitoring stations

## 🌫️ The Problem: When Air Becomes Invisible Danger

- **In Ratchaburi Province, Thailand**, like many communities worldwide, air pollution is a silent crisis.

- **PM2.5 particles** — so small they penetrate deep into lungs and bloodstreams — remain invisible to human eyes while causing serious health impacts.

**Official monitoring stations** are few and far between, leaving communities with little understanding of their daily exposure. Installing professional monitoring networks is prohibitively expensive, often costing thousands per station.

![Hotspot+PM25plot](https://github.com/user-attachments/assets/771f9c1c-860d-4a40-9879-c0ff2a622690)

*PM 2.5 and Fires in Central Thailand – March 2024*

### 🎨 Our Solution: Art Meets Science & Mobile Community Monitoring

**We created PM2.5 Ghostbuster** to bridge the gap between hard science and public understanding.

**👻 The "Ghost" Metaphor** — By treating PM2.5 pollution as invisible "ghosts" that need detection, we use art and cultural approaches to make environmental data accessible and actionable.

**🤝 Cross-Cultural Collaboration** — This project represents a meeting of German engineering expertise and Thai community arts, demonstrating how maker culture and creative technology can empower communities to monitor and respond to environmental challenges on their own terms.

**🛠️ Built Together** — During the five-day artist residency, community members learned to hack consumer electronics, build sensors, and create their own environmental monitoring tools.

**💡 Smart Economics** — Instead of expensive fixed monitoring stations, mobile sensors carried by community members (vendors, commuters, delivery riders) map pollution across entire neighborhoods. Offline data collection means anyone can contribute—even without continuous internet access. When devices sync at home or public hotspots, the entire community gains visibility into local air quality patterns.


## 🔧 The Magic: From €12 IKEA Sensor to Smart Detector

### 🏠 Step 1: Hacking Consumer Hardware

**The magic begins** with a clever hack of consumer hardware.

**Inside each IKEA VINDRIKTNING** sits a professional-grade particle detector (Cubic PM1006) that IKEA uses to power its simple green-yellow-red indicator lights.

**We tap into** the sensor's internal communication channel—called UART—to access the raw numerical data that the sensor produces every second.

![Hardware schematic showing sensor modification](https://github.com/user-attachments/assets/72173833-a505-4313-9137-12b720856811)

*Hardware schematic by Chiara Giardi showing the ESP8266 microcontroller and GPS integration*

### Step 2: Adding Intelligence

**Into the original IKEA housing**, we install:
- 🔌 **ESP8266 microcontroller** (about the size of a thumb) — the device's brain
- 📍 **GPS module** — tracks location as you move
- 📶 **WiFi connectivity** — sends data to the cloud or stores it locally

![Inside the mobile PM2.5 detector device](https://github.com/user-attachments/assets/3ce0eb01-d379-40ea-821c-2233dbe41414)

*The enhanced internals: ESP8266 brain, GPS eyes, and the original IKEA sensor*

**The microcontroller:**
1. Reads PM2.5 measurements every second
2. Combines them with GPS coordinates
3. Packages everything as structured data
4. Transmits to cloud server using MQTT protocol (when WiFi available)
5. Stores locally when offline, syncs automatically later

### ☁️ Step 3: Cloud Infrastructure (The Network Brain)

**A Python-based server** receives data from all active devices and:
- ✅ Validates incoming measurements
- 💾 Stores in a time-series database (InfluxDB)
- 🗺️ Generates GeoJSON file every minute
- 🌐 Feeds real-time web map (Leaflet.js)
- 📊 Displays 30 days of historical air quality patterns

### 🚀 Step 4: Smart Features for Mobile Monitoring

**Movement Detection** 🏃
Only publishes new data after traveling 30+ meters → conserves battery and data

**Offline Resilience** 💾
Device stores measurements in flash memory when no internet → auto-syncs when connection returns → no pollution data ever lost

**Community-Powered Coverage** 🏘️
Mobile vendors, ice cream bike-shops, and commuters become environmental sensors → collect data across neighborhoods while offline → sync at home/public WiFi → entire community sees pollution hotspots on shared map → replaces need for expensive fixed monitoring stations


## Key Features

- **🏠 Consumer Hardware Foundation** - Built on €12 IKEA VINDRIKTNING sensors, making professional environmental monitoring accessible
- **📍 GPS-Enabled Mobile Monitoring** - Track air quality along routes, revealing pollution patterns across neighborhoods and cities
- **💾 Intelligent Offline Operation** - Automatically stores PM2.5 data, timestamps, and GPS coordinates locally when offline, syncs when connection restores—perfect for mobile vendors and commuters without constant internet
- **🌐 Real-Time Global Visualization** - Live web map shows all measurements from all devices with 30 days of historical data, publicly accessible to entire community
- **📱 Smartphone Integration** - Works with phone mobile hotspots for instant data sharing, or operates fully offline until reaching WiFi
- **☁️ Professional Cloud Infrastructure** - Enterprise-grade Python backend with automated alerts and REST API
- **🔓 Fully Open Source** - Complete hardware modifications, firmware, and server code available for replication
- **🌍 Community-Powered Network** - Mobile sensors carried by community members map pollution hotspots across entire neighborhoods, eliminating need for expensive fixed monitoring stations
- **💰 Cost-Effective Coverage** - One €30 mobile device can cover areas requiring thousands of dollars in traditional fixed sensors

## If you need to understand more

### What It Does

Imagine carrying a device no bigger than a smartphone that reveals the invisible—microscopic particles floating in the air around you, potentially harming your health with every breath. PM2.5 Ghostbuster transforms affordable IKEA air quality sensors into mobile environmental detectives that track pollution as you move through your city, your neighborhood, your daily commute.

When you mount one of these modified sensors on your motorcycle or carry it while walking, it continuously measures the air quality and plots the data on a live map accessible to anyone with an internet connection. The invisible becomes visible: streets that glow red with dangerous pollution levels, neighborhoods where the air breathes clean and green, patterns that emerge showing how pollution moves through communities.

What makes this system powerful is its accessibility. The heart of each device is a €12 IKEA VINDRIKTNING air quality sensor—designed by David Wahl for home use—that we've carefully enhanced with GPS tracking and WiFi connectivity. When you connect the device to your phone's hotspot, it instantly shares data to a global map. Even when internet isn't available, it remembers everything locally and syncs automatically when connection returns. This isn't just environmental monitoring—it's environmental democracy, putting professional-grade air quality tracking into the hands of anyone who wants to understand the air they breathe.


### Why It Exists

In Ratchaburi Province, Thailand, like many communities worldwide, air pollution is a silent crisis. PM2.5 particles—so small they penetrate deep into lungs and bloodstreams—remain invisible to human eyes while causing serious health impacts. Official monitoring stations are few and far between, leaving communities with little understanding of their daily exposure.

We created PM2.5 Ghostbuster to bridge the gap between hard science and public understanding. By treating PM2.5 pollution as invisible "ghosts" that need detection, we use art and cultural approaches to make environmental data accessible and actionable. This project represents a collaboration between German engineering expertise and Thai community arts, demonstrating how maker culture and creative technology can empower communities to monitor and respond to environmental challenges on their own terms.


### How It Works

The magic begins with a clever hack of consumer hardware. Inside each IKEA VINDRIKTNING sensor sits a professional-grade particle detector (Cubic PM1006) that IKEA uses to power its simple green-yellow-red indicator lights. We've learned to tap into the sensor's internal communication channel—called UART—to access the raw numerical data that the sensor produces every second.

Into the original IKEA housing, we install a tiny ESP8266 microcontroller (about the size of a thumb) and a GPS module. The microcontroller reads the PM2.5 measurements, combines them with GPS coordinates from the location module, and packages everything as structured data. When connected to WiFi—either from a home network or your phone's mobile hotspot—it transmits this data package to a cloud server using a lightweight protocol called MQTT.

The cloud infrastructure acts as the system's memory and brain. A Python-based server receives data from all active devices, validates it, and stores it in a time-series database optimized for environmental measurements. Every minute, the server generates a GeoJSON file—a standardized geographic data format—that feeds a real-time web map built with Leaflet.js. This map becomes the public face of the project: an interactive visualization where anyone can see current and historical air quality patterns spanning 30 days of data.

The system includes intelligent features that make mobile monitoring practical. Movement detection ensures the device only publishes new data when it has traveled a meaningful distance (typically 30 meters), conserving battery and data. When internet connectivity drops—common during mobile operation—the device stores measurements locally in its flash memory and automatically syncs them when connection returns. This resilience means no pollution data is ever lost, even in areas with spotty coverage.


## Getting Started

### For Community Members & Artists

**What You Need:**
- IKEA VINDRIKTNING Air Quality Sensor ([available at IKEA stores](https://www.ikea.com/in/en/p/vindriktning-air-quality-sensor-80498246/), Product ID: 804.982.46)
- ESP8266 development board (Wemos D1 Mini or NodeMCU, ~$3-5)
- GPS module (Neo-6M, ~$8-10)
- USB-C cable and power bank for mobile operation
- Basic soldering skills and jumper wires

**Building Your Device:**
1. Open the IKEA sensor housing (gentle prying, no damage to internals)
2. Connect ESP8266 to the sensor's UART pins (detailed wiring diagrams available)
3. Add GPS module for location tracking
4. Upload pre-configured firmware via USB
5. Power on and connect to WiFi using smartphone configuration portal

**Using Your Device:**
- Mount on motorcycle/bicycle or carry in backpack with power bank
- Connect device to your phone's WiFi hotspot
- Watch your measurements appear on the live map within seconds
- Share the map link with your community

**Workshop Opportunities:**
We regularly conduct hands-on building workshops. Contact [Baan Noorg Collaborative Arts & Culture](https://www.baannoorg.com/) for upcoming sessions.



## Key Results & Impact

### Live Monitoring Network

![516838813_10171998375205074_9010611628466192082_n](https://github.com/user-attachments/assets/2fbb14e4-5dd6-4da1-8b86-ba49c21ff693)

*DIY Air-Quality Workshop: Building a PM2.5 Detector at Baan Noorg community in Ratchaburi Province - May 2023*


![Visualization showing pollution patterns](https://github.com/user-attachments/assets/d527273f-41f9-4cb3-95e9-1d0747295254)

*"Flower of pollution"—radial patterns showing daily PM2.5 variations across monitoring routes*


![Dense measurement data showing air quality patterns](https://github.com/user-attachments/assets/ff23e490-b14a-48f6-bedd-2b305d3948cc)

*High-density PM2.5 measurements revealing pollution hotspots in Ratchaburi Province*


### Real-World Applications

- **🏍️ Commuter Exposure Mapping** - Motorcycle riders track their daily pollution exposure while offline, data syncs at home
- **🛒 Mobile Vendor Monitoring** - Ice cream bikes, food carts, and delivery riders collect pollution data across neighborhoods during work hours, contributing to community maps without needing constant internet
- **🏘️ Neighborhood Coverage** - Communities identify local pollution sources and patterns without expensive fixed monitoring stations
- **📊 Citizen Science Data** - Distributed mobile monitoring network provides comprehensive data where official stations don't exist
- **🎓 Educational Tool** - Schools use devices to teach environmental science hands-on
- **🗣️ Advocacy Evidence** - Data supports community conversations with local authorities, showing pollution patterns across entire neighborhoods


## Project Credits

### Core Collaboration
- **Werapol Bejranonda** (Germany) - Engineering, System Architecture, Hardware Design
- **Baan Noorg Collaborative Arts and Culture** (Thailand) - Host Organization, Community Engagement, Cultural Translation

### Key Collaborators
- **Chiara Giardi** - Curation, Project Interpretation, Continuator
- **Nisa Jewcharoen** - Coordination and Community Liaison

![349743525_787367736076461_4508559546060926412_n](https://github.com/user-attachments/assets/4803fc9a-251d-4cfb-8939-b13086559576)
*Workshop planning with curator Chiara Giardi and engineer Werapol Bejranonda*

### Local Team & Participants
- **JiandYin, Ploy, Im, Boss** - Workshop facilitation and device testing

### Community
Local residents of Ratchaburi Province for participation, testing, and ongoing monitoring

![Workshop participants building mobile PM2.5 detectors](https://github.com/user-attachments/assets/a1335e2e-155a-48e6-ab0a-547eb582a91e)

*Community workshop at Baan Noorg teaching sensor construction and environmental monitoring*

### Technical Foundation
This project builds upon [**Hypfer's esp8266-vindriktning-particle-sensor**](https://github.com/Hypfer/esp8266-vindriktning-particle-sensor) (Apache-2.0), which pioneered MQTT connectivity for IKEA VINDRIKTNING sensors. Our enhancements add GPS integration, offline storage, intelligent publishing algorithms, and mobile optimization while maintaining the Apache-2.0 license.

### Open Source Libraries
- **TinyGPSPlus** (Mikal Hart) - GPS parsing
- **WiFiManager** (tzapu) - Configuration portal
- **PubSubClient** - MQTT communication
- **Leaflet.js** - Interactive mapping
- **InfluxDB & Paho-MQTT** - Backend infrastructure

### Hardware Attribution
**IKEA VINDRIKTNING** - Designed by David Wahl for IKEA (Product ID: 804.982.46)
Internal sensor: Cubic PM1006 particulate matter detector


## License & Usage

This project is licensed under **Apache License 2.0** - see [LICENSE](LICENSE) for details.

**What This Means:**
- ✅ Free to use for personal, educational, or community projects
- ✅ Modify and adapt the hardware/software for your needs
- ✅ Use in art installations, exhibitions, or public interventions
- ✅ Create commercial applications (with proper attribution)
- ⚖️ Patent protection provided for all contributors
- 📄 Must preserve copyright notices and license information

**We Encourage:**
- Replicating this system in your community
- Teaching workshops using our designs
- Contributing improvements back to the project
- Documenting your local environmental findings
- Sharing your story with us


## For Developers

<!-- TECHNICAL DETAIL -->

<details>
<summary><strong>🏗️ Complete System Architecture (v3.0.0)</strong></summary>

### System Overview: Three Integrated Components

PM2.5 Ghostbuster is a unified environmental monitoring ecosystem consisting of:

1. **📱 Mobile Hardware** (Arduino/) - ESP8266-based sensors with GPS
2. **☁️ Cloud Backend** (Server/) - Python services with MQTT, InfluxDB, REST API
3. **🌐 Web Visualization** (Leaflet/) - Real-time mapping interface

### Data Flow Architecture

```
IKEA VINDRIKTNING → ESP8266 → GPS → Local Storage ⟷ WiFi → MQTT → Cloud → Leaflet Map
     ↓                 ↓        ↓         ↓           ↓       ↓       ↓         ↓
Internal PM2.5 → Process → Location → Cache → Phone/WiFi → Broker → Server → Real-time Web
```

### MQTT Communication Protocol

**Topic Structure:**
```
pm25/{deviceID}/air          # Main data stream
pm25/{deviceID}/CMND         # Commands to device
pm25/{deviceID}/LWT          # Last Will Testament (offline status)
```

**Message Format (JSON):**
```json
{
  "_type": "location",
  "lat": 13.7563,          // GPS latitude
  "lon": 100.5018,         // GPS longitude
  "pm25": 35,              // PM2.5 reading (μg/m³)
  "tst": 1640995200,       // Unix timestamp
  "tid": "AB",             // Device ID (last 2 chars)
  "t": "v"                 // Type: 'v'=vehicle, 'p'=ping, 'f'=first_fix
}
```

</details>

<details>
<summary><strong>📱 Arduino Firmware Implementation</strong></summary>

### Production Implementations

The `Arduino/` directory contains two production-ready implementations:

#### **1. Fixed Location Sensors** (`picoPM25bFixedCoord/`)
For stationary monitoring without GPS hardware:
- Uses predefined coordinates in configuration
- Optimized for static monitoring locations
- WiFiManager captive portal for easy setup
- Lower power consumption (no GPS)

#### **2. Mobile GPS Sensors** (`picoPM25v4/`)
For mobile monitoring with location tracking:
- Real-time GPS positioning (Neo-6M module)
- Movement-based intelligent publishing (30m threshold)
- Offline SPIFFS storage with auto-sync
- OwnTracks protocol compatibility
- Battery-powered mobile operation

### Key Firmware Modules

**SerialCom.h** - VINDRIKTNING UART communication
- Parses PM1006 sensor data stream
- Handles serial timing and validation
- Provides moving average calculations

**Types.h** - Data structures and state management
- Device state tracking
- Measurement buffering
- Connection status management

**pico.h** - Configuration header
- WiFi credentials
- MQTT broker settings
- GPS configuration
- Publishing thresholds

### Offline Resilience

```cpp
// Automatic offline storage when no internet
if (!WiFi.connected()) {
    store_locally(pm25_data, gps_location, timestamp);
} else {
    upload_to_cloud(data);
    upload_cached_data(); // Sync stored data
}
```

### Movement Detection Algorithm

```cpp
// Smart publishing based on distance traveled
float distance = TinyGPSPlus::distanceBetween(
    last_lat, last_lon,
    current_lat, current_lon
);

if (distance >= PUBLISH_DISTANCE) {
    serialize(gps, 'v');  // 'v' = vehicle/movement
    client.publish(pubtopic, payload, true);
    update_last_position();
}
```

</details>

<details>
<summary><strong>🖥️ Server Backend Architecture</strong></summary>

### Modular Python Services

**Configuration Management** (`config/`)
- `settings.py` - Centralized configuration loader
- `.env.example` - Secure environment variable template
- Environment-based credentials (no hardcoded secrets)

**Core Services** (`src/services/`)
- `mqtt_service.py` - MQTT client with auto-reconnection
- `influx_service.py` - InfluxDB operations with batch processing
- `geojson_service.py` - GeoJSON generation for map visualization
- `alert_service.py` - WHO/EPA-based PM2.5 threshold alerting
- `api_service.py` - Flask REST API endpoints

**Data Models** (`src/models/`)
- `air_quality.py` - Measurement validation and transformation

**Applications**
- `main_data_collector.py` - Primary service (MQTT subscriber + API server)
- `main_mqtt_logger.py` - Debug logging service

### Server Setup

```bash
cd Server

# Install dependencies
pip install -r requirements.txt

# Configure environment
cp config/.env.example config/.env
# Edit .env with your credentials

# Run main data collector (includes API)
python3 src/main_data_collector.py

# Or use PM2 for process management
pm2 start ecosystem.config.js
```

### Database Schema

**InfluxDB 1.x Configuration:**
- Database: `pm25gps`
- Measurement: `air_quality`
- Fields: PM2.5 values, GPS coordinates, timestamps, device IDs
- Retention: 720 hours (30 days)

**Query Example:**
```python
query = '''
SELECT mean("pm25") as pm25, "lat", "lon", "device_id"
FROM "air_quality"
WHERE time > now() - 720h
GROUP BY time(5m), "device_id"
'''
```

### REST API Endpoints

```bash
# Health check
GET /api/v1/health

# Latest readings from all devices
GET /api/v1/readings/latest

# Historical data for specific device
GET /api/v1/readings/device/{device_id}?hours=24

# System statistics
GET /api/v1/stats
```

### Alert System

**WHO/EPA PM2.5 Thresholds:**
```python
PM25_THRESHOLDS = {
    'good': (0, 12),
    'moderate': (12, 35),
    'unhealthy': (35, 55),
    'very_unhealthy': (55, 150),
    'hazardous': (150, 500)
}
```

Configurable notifications via SMTP when readings exceed thresholds.

</details>

<details>
<summary><strong>🗺️ Web Visualization Frontend</strong></summary>

### Leaflet.js Implementation

**Features:**
- Real-time GeoJSON data updates (60-second refresh)
- Marker clustering for dense data visualization
- Heat map overlays with color coding by PM2.5 levels
- Historical data slider (720 hours)
- Mobile-responsive design
- Full-screen mapping capability
- GPS location services for user positioning

**Data Source:**
```javascript
// Auto-updating GeoJSON feed
const dataUrl = '/gj/pm25gps.geojson';
setInterval(() => {
    fetch(dataUrl)
        .then(response => response.json())
        .then(data => updateMap(data));
}, 60000); // Update every 60 seconds
```

**Color Coding:**
```javascript
function getColor(pm25) {
    return pm25 > 150 ? '#7e0023' :    // Hazardous
           pm25 > 55  ? '#8f3f97' :    // Very Unhealthy
           pm25 > 35  ? '#ff0000' :    // Unhealthy
           pm25 > 12  ? '#ff7e00' :    // Moderate
                        '#00e400';     // Good
}
```

</details>

<details>
<summary><strong>🔧 Development & Deployment</strong></summary>

### Hardware Setup

**Required Components:**
- IKEA VINDRIKTNING (€12)
- ESP8266 D1 Mini / NodeMCU ($3-5)
- GPS Module Neo-6M ($8-10, optional for mobile version)
- Jumper wires, USB-C cable
- Power bank for mobile operation

**Wiring Connections:**
```
VINDRIKTNING → ESP8266
    REST (UART TX) → D2 (GPIO4)
    5V → 5V
    GND → GND

GPS Module → ESP8266 (picoPM25v4 only)
    TX → D4 (GPIO2)
    RX → D3 (GPIO0)
    VCC → 3.3V
    GND → GND
```

### Software Requirements

**Arduino Environment:**
- Arduino IDE 1.8+ or PlatformIO
- ESP8266 board package (3.0+)
- Libraries: TinyGPSPlus, WiFiManager, PubSubClient, ArduinoJson

**Server Infrastructure:**
- Python 3.8+
- InfluxDB 1.x
- MQTT broker (Mosquitto)
- Web server (Apache/Nginx)

### Deployment Commands

**Arduino Upload:**
```bash
# Configure credentials
cp Arduino/picoPM25v4/pico.h.example Arduino/picoPM25v4/pico.h
# Edit pico.h with WiFi/MQTT settings

# Upload via Arduino IDE or:
arduino-cli compile --fqbn esp8266:esp8266:d1_mini Arduino/picoPM25v4
arduino-cli upload -p /dev/ttyUSB0 --fqbn esp8266:esp8266:d1_mini
```

**Server Deployment:**
```bash
cd Server

# Automated setup
./scripts/setup.sh

# Manual setup
pip install -r requirements.txt
cp config/.env.example config/.env
# Edit .env file

# Start with PM2
pm2 start ecosystem.config.js
pm2 save
pm2 startup
```

**Web Interface:**
```bash
# Copy to web root
cp -r Leaflet/* /var/www/html/

# Ensure GeoJSON output path is writable
mkdir -p /var/www/html/gj
chown www-data:www-data /var/www/html/gj
```

### Testing & Debugging

**Verify MQTT Connection:**
```bash
# Subscribe to all device messages
mosquitto_sub -h mqtt.thalay.eu -t "pm25/+/air" -u pm25 -P <password>
```

**Query InfluxDB:**
```bash
influx -database pm25gps
> SELECT * FROM air_quality ORDER BY time DESC LIMIT 10
> SELECT COUNT(*) FROM air_quality WHERE time > now() - 24h
```

**API Health Check:**
```bash
curl http://localhost:5000/api/v1/health
```

</details>

<details>
<summary><strong>📚 Project Structure & Version History</strong></summary>

### Repository Organization

```
PM25-Ghostbuster/
├── Arduino/                        # Mobile sensor firmware
│   ├── picoPM25bFixedCoord/       # Fixed location sensors
│   ├── picoPM25v4/                # Mobile GPS sensors
│   └── esp8266-vindriktning-particle-sensor/  # Original Hypfer implementation
├── Server/                         # Backend services
│   ├── config/                    # Configuration management
│   ├── src/                       # Source code
│   │   ├── services/              # Business logic layer
│   │   ├── models/                # Data models
│   │   └── utils/                 # Utilities
│   ├── scripts/                   # Automation tools
│   └── ecosystem.config.js        # PM2 configuration
├── Leaflet/                       # Web visualization
│   ├── index.html                 # Main interface
│   ├── css/                       # Stylesheets
│   └── js/                        # JavaScript
├── API.md                         # API documentation
├── CONFIGURATION.md               # Configuration guide
└── DEPLOYMENT.md                  # Deployment guide
```

### Version History

**v3.0.0 (2025)** - Historical Integration & Complete Documentation
- Complete integration of original 2023 implementation
- Unified development narrative from prototype to enterprise
- Educational documentation of IoT sensor hacking techniques
- Community impact recognition and attribution

**v2.1.0 (2025)** - Professional Features
- Advanced alert system with WHO/EPA standards
- REST API for data access and system management
- Automation scripts and health monitoring
- PM2 process management integration

**v2.0.0 (2025)** - Modular Architecture
- Security improvements (no hardcoded credentials)
- Modular service-based architecture
- Enhanced error handling and auto-recovery
- Professional logging and monitoring

**v1.x (2023)** - Original Prototype
- VINDRIKTNING hacking and UART access
- GPS integration for mobile monitoring
- MQTT connectivity and real-time mapping
- Community workshops and initial deployment

</details>

<details>
<summary><strong>🤝 Contributing & Replication</strong></summary>

### How to Contribute

We welcome contributions from:
- **Hardware hackers** - Improve sensor modifications and power management
- **Software developers** - Enhance backend services and data processing
- **Data scientists** - Develop analysis tools and visualization improvements
- **Community organizers** - Document deployment experiences and workshop curricula
- **Artists** - Create new ways to visualize and interpret environmental data

**Contribution Process:**
1. Fork the repository
2. Create a feature branch
3. Make your improvements with clear documentation
4. Test thoroughly (hardware/software)
5. Submit pull request with detailed description

### Replicating in Your Community

**Step 1: Assess Your Context**
- Identify local air quality concerns
- Find community partners (schools, arts spaces, environmental groups)
- Estimate budget (~€30-50 per device)

**Step 2: Build Pilot Devices**
- Order components (IKEA sensor + ESP8266 + GPS)
- Follow hardware modification guides
- Test in controlled environment

**Step 3: Infrastructure Setup**
- Deploy server backend (cloud or local)
- Configure MQTT broker and database
- Set up public web map

**Step 4: Community Engagement**
- Organize building workshops
- Train community members in device use
- Establish data sharing protocols
- Plan ongoing maintenance

**Step 5: Data Action**
- Share findings with local authorities
- Use data for community advocacy
- Integrate with existing environmental initiatives
- Document and share your story

### Support & Resources

- **Technical Documentation:** See [DEPLOYMENT.md](DEPLOYMENT.md) and [CONFIGURATION.md](CONFIGURATION.md)
- **Hardware Guides:** Detailed wiring diagrams in Arduino directories
- **Community Forum:** GitHub Discussions for questions and sharing
- **Workshop Materials:** Contact Baan Noorg for curricula and facilitation guides

</details>

---

## Related Projects

This project is part of a broader practice exploring the intersection of art, technology, and community engagement. Explore other works:

- **[The Not-So-Modern Dictionary](https://github.com/bejranonda/the-not-so-modern-dictionary)** - Interactive language installation
- **[Code of the Sea](https://github.com/bejranonda/code-of-the-sea)** - Raspberry Pi performance control system
- **[Heating DJ](https://github.com/bejranonda/Heating-DJ)** - Thermal imaging music system

---

*Making the invisible visible, one sensor at a time.* 👻

**Questions or collaboration?** Open an issue or reach out through [Baan Noorg](https://www.baannoorg.com/)
