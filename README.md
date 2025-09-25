# PM2.5 Ghostbuster

> *"PM2.5 is like ghost. It exists even if it is invisible."*

A collaborative art-science project making invisible air pollution visible through DIY sensors, real-time data visualization, and community engagement.

## 🌍 About

PM2.5 Ghostbuster is a joint initiative between **Werapol Bejranonda** (Engineer - Germany) and **Baan Noorg Collaborative Arts and Culture** (Artist Collective - Thailand) focused on raising awareness about PM2.5 air pollution in Rajabura province, Thailand.

The project bridges the gap between hard science and public understanding by using art and cultural approaches to make environmental data accessible to local communities.

**Live Data**: [map.thalay.eu](https://map.thalay.eu/)

## 🎯 Project Philosophy

This project transforms complex environmental science into understandable, actionable information for civilians. By treating PM2.5 pollution as an invisible "ghost" that needs to be detected and understood, we empower communities to monitor and respond to their air quality.

## 🛠 Technical Architecture

### Arduino Sensor Device (`Arduino/`)
- **File**: `pm25ikea.cpp`
- ESP8266-based DIY PM2.5 sensor with GPS tracking
- Features:
  - Real-time PM2.5 measurement
  - GPS location tracking
  - MQTT data transmission
  - Portable and easy to build
  - WiFi connectivity with automatic reconnection

### Server Backend (`Server/pm25/`)
- **Python Scripts**:
  - `PM25ikeav2.py` - Main MQTT subscriber and InfluxDB data processor
  - `GetDataFromPM25ikea.py` - Data collection from sensors
  - `PM25mqttLog.py` - MQTT logging functionality
  - `CSVdownloadMap.js` - CSV data export functionality

### Web Visualization (`Leaflet/`)
- Interactive web mapping interface built with Leaflet.js
- **Features**:
  - Real-time PM2.5 data visualization
  - Heat map overlays
  - Marker clustering for dense data
  - Historical data viewing (up to 720 hours)
  - Mobile-responsive design
  - Full-screen mapping capability
  - GPS location services

## 📊 Data Flow

1. **Collection**: Arduino devices measure PM2.5 levels and GPS coordinates
2. **Transmission**: Data sent via MQTT to cloud server
3. **Storage**: InfluxDB stores time-series air quality data
4. **Processing**: Python scripts generate GeoJSON files every minute
5. **Visualization**: Web interface displays real-time and historical data

## 🚀 Getting Started

### Prerequisites
- ESP8266 development board (for sensor device)
- PM2.5 sensor module
- GPS module
- InfluxDB database
- MQTT broker
- Python 3.x with required packages

### Sensor Device Setup
1. Configure WiFi credentials in `pico.h`
2. Set MQTT broker settings
3. Upload `pm25ikea.cpp` to ESP8266
4. Deploy device in monitoring location

### Server Setup
1. Install InfluxDB and configure database `pm25gps`
2. Set up MQTT broker (default: `mqtt.thalay.eu`)
3. Install Python dependencies:
   ```bash
   pip install paho-mqtt influxdb geojson pytz
   ```
4. Run the data collection services:
   ```bash
   pm2 start PM25ikeav2.py --interpreter python3
   pm2 start GetDataFromPM25ikea.py
   ```

### Web Interface
1. Deploy Leaflet files to web server
2. Configure GeoJSON data path in `index.html`
3. Access via web browser

## 📁 Project Structure

```
PM25-Ghostbuster/
├── Arduino/
│   └── pm25ikea.cpp          # ESP8266 sensor firmware
├── Server/pm25/
│   ├── PM25ikeav2.py         # Main data processing service
│   ├── GetDataFromPM25ikea.py # Data collection service
│   ├── PM25mqttLog.py        # MQTT logging
│   ├── CSVdownloadMap.js     # CSV export functionality
│   └── pm25gps.geojson       # Generated location data
└── Leaflet/
    ├── index.html            # Main web interface
    ├── css/                  # Stylesheets
    ├── js/                   # JavaScript libraries
    └── img/                  # Images and assets
```

## 🎨 Art-Science Integration

This project demonstrates how technology can be made accessible through creative approaches:
- **Metaphor**: Treating PM2.5 as invisible "ghosts" that need detection
- **Community Focus**: Empowering locals with their own monitoring capabilities
- **Visual Storytelling**: Interactive maps tell the story of air quality over time
- **Cultural Bridge**: Connecting scientific data with cultural understanding

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