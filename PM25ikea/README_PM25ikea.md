# PM25ikea - Original VINDRIKTNING Implementation (2023)

> **Historical Implementation**: Original IKEA VINDRIKTNING hacking scripts that pioneered the PM2.5 Ghostbuster concept

## 🏗 Overview

This folder contains the **original Arduino implementation** developed in 2023 that first demonstrated the feasibility of hacking IKEA's VINDRIKTNING air quality sensor for mobile environmental monitoring. These scripts established the foundation for the current enterprise-grade PM2.5 Ghostbuster system.

## 🔧 Hardware Configuration

### Core Components
- **IKEA VINDRIKTNING** - PM2.5 air quality sensor (€12)
- **ESP8266 D1 Mini** - WiFi microcontroller
- **GPS Module (Neo-6M)** - Location tracking
- **Power Bank** - 5V mobile power supply

### Pin Configuration (Original Setup)
```
ESP8266 D1 Mini:
├── D1 (GPIO5)  → GPS TX
├── D2 (GPIO4)  → GPS RX
├── D4 (GPIO2)  → PM2.5 Sensor UART
├── D5 (GPIO14) → Status LED
└── VIN         → 5V Power (USB-C from VINDRIKTNING)
```

## 📁 Key Scripts

### 🎯 **Main Implementations**

#### `PM25gps/PM25gps.ino`
**Original GPS+PM2.5 monitoring script**
- **Features**: Combined GPS tracking with PM2.5 measurements
- **Protocol**: OwnTracks-compatible JSON publishing
- **GPS Integration**: Real-time location with time synchronization
- **Data Format**: JSON with lat/lon/pm25/timestamp
- **MQTT Topics**: `pm25/{deviceID}/air`

#### `picoPM25v4/picoPM25v4.ino`
**Advanced implementation with WiFiManager**
- **Features**: Enhanced connectivity and configuration management
- **WiFiManager**: Captive portal for easy WiFi setup
- **Auto-reconnection**: Automatic WiFi and MQTT recovery
- **Smart Publishing**: Movement-based data transmission
- **Offline Storage**: SPIFFS filesystem for data caching
- **Device ID**: Hardware-based unique identification

### 🔄 **Development Versions**
- `picoPM25b/` - Early development version
- `picoPM25bFixedCoord/` - Coordinate system improvements
- `PicoPM25/picoPM25/` - Initial prototypes

## ⚡ **Technical Innovations (2023)**

### 1. **UART Communication**
Direct access to VINDRIKTNING's internal PM2.5 sensor data through UART protocol
```cpp
constexpr static const uint8_t PIN_UART_RX = 2; // D4 on Wemos D1 Mini
SoftwareSerial sensorSerial(PIN_UART_RX, PIN_UART_TX);
```

### 2. **Dual Serial Management**
Simultaneous GPS and PM2.5 sensor communication
```cpp
SoftwareSerial nss(RXpin, TXpin);  // GPS on D1/D2
SoftwareSerial sensorSerial(PIN_UART_RX, PIN_UART_TX); // PM2.5 on D4
```

### 3. **Smart Movement Detection**
Distance-based publishing to optimize data usage
```cpp
int mindist = 50; // minimum distance in meters before publishing
unsigned long meters = TinyGPSPlus::distanceBetween(
    gps.location.lat(), gps.location.lng(),
    last_lat, last_lon
);
if (meters_since_pub >= mindist) {
    serialize(gps, 'v'); // Publish vehicle/movement data
    meters_since_pub = 0;
}
```

### 4. **Offline Data Storage**
SPIFFS filesystem for connection resilience
```cpp
#define STORE "/data/owntracks.log"
void store(char *payload) {
    File f = SPIFFS.open(STORE, "a");
    f.println(payload);
    f.close();
}
```

### 5. **OwnTracks Compatibility**
GPS tracking protocol for integration with existing systems
```cpp
doc["_type"] = "location";
doc["lat"] = gps.location.lat();
doc["lon"] = gps.location.lng();
doc["pm25"] = int(state.avgPM25);
doc["tst"] = now(); // Unix timestamp
```

## 🌍 **Mobile Environmental Monitoring**

### Original Use Case
These scripts enabled the creation of **mobile air quality sensors** that could:
- Track pollution levels while moving through different areas
- Create real-time pollution maps
- Store data offline when connectivity was poor
- Automatically upload cached data when connection restored

### Data Flow (2023)
```
VINDRIKTNING → UART → ESP8266 → GPS → WiFi → MQTT → InfluxDB → Web Map
     ↓
Internal PM2.5 → Raw Data → Location Tag → JSON → Cloud → Visualization
```

## 🔗 **Integration with Current System**

### Evolution to v2.x
The concepts pioneered in these original scripts evolved into the current enterprise-grade system:

**2023 Original** → **2025 v2.x Enhanced**
- Basic WiFi → Professional WiFiManager with recovery
- Simple MQTT → Enterprise MQTT with guaranteed delivery
- GPS tracking → Enhanced GPS with smart publishing
- File storage → Professional database integration
- Manual config → Secure environment-based configuration

### Legacy Value
While the current `Arduino/` folder contains the production-ready v2.x implementation, these original scripts remain valuable for:
- **Educational purposes**: Understanding the evolution of the project
- **Research reference**: Studying original implementation approaches
- **Alternative configurations**: Different hardware setups or requirements
- **Historical documentation**: Preserving the development journey

## 🚀 **Quick Start (Historical)**

### Prerequisites (2023 Setup)
1. Arduino IDE with ESP8266 board package
2. Required libraries:
   - WiFiManager
   - PubSubClient (MQTT)
   - TinyGPS++
   - ArduinoJson
   - TimeLib

### Configuration
1. Edit `pico.h` with WiFi and MQTT credentials:
```cpp
#define WIFISSID "YourWiFi"
#define WIFIPASSWORD "YourPassword"
#define MQTTHOST "mqtt.yourserver.com"
```

2. Upload to ESP8266 D1 Mini
3. Connect hardware according to pin configuration
4. Power via USB-C (VINDRIKTNING power supply)

## 📊 **Historical Data Format**

### JSON Output (OwnTracks Compatible)
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

### MQTT Topics
- **Data**: `pm25/{deviceID}/air`
- **Commands**: `pm25/{deviceID}/CMND`
- **Last Will**: `pm25/{deviceID}/LWT`

## 🏆 **Legacy Achievement**

These original scripts successfully demonstrated:
- **€12 Consumer Device** → **Professional IoT Sensor**
- **Static Air Monitor** → **Mobile Environmental Mapping**
- **Local Display Only** → **Real-time Cloud Integration**
- **Individual Device** → **Sensor Network Foundation**

This pioneering work in 2023 established the technical and conceptual foundation for the current PM2.5 Ghostbuster project, proving that affordable consumer electronics could be transformed into powerful environmental monitoring tools for community empowerment.

---

*Original development: 2023 | Documentation: 2025 | Status: Historical Reference*