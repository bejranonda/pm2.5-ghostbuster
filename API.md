# PM2.5 Ghostbuster - REST API Documentation

**Version 2.1.0**

The PM2.5 Ghostbuster REST API provides comprehensive access to real-time air quality data, system monitoring, and alert management.

## 📖 **API Overview**

- **Base URL**: `http://your-server:5000/api/v1`
- **Content-Type**: `application/json`
- **CORS**: Enabled for web frontend integration
- **Authentication**: None required (configure as needed for production)

## 🔗 **Endpoints**

### 🏥 **System Health**

#### `GET /health`
Comprehensive system health check

**Response:**
```json
{
  "status": "healthy",
  "timestamp": "2025-09-25T10:30:00+07:00",
  "services": {
    "influxdb": true,
    "geojson": true,
    "alerts": true
  },
  "healthy": true,
  "version": "2.1.0"
}
```

**Status Codes:**
- `200` - System healthy
- `503` - System unhealthy

---

### 📊 **Data Access**

#### `GET /data/current`
Get current air quality data in GeoJSON format

**Parameters:**
- `hours` (optional): Hours of data to retrieve (1-168, default: 24)

**Example:**
```bash
curl "http://localhost:5000/api/v1/data/current?hours=48"
```

**Response:**
```json
{
  "type": "FeatureCollection",
  "features": [
    {
      "type": "Feature",
      "geometry": {
        "type": "Point",
        "coordinates": [99.914530, 13.741263]
      },
      "properties": {
        "device_id": "sensor001",
        "pm25": 35.2,
        "time": "2025-09-25 10:25:00",
        "speed": 0.0
      }
    }
  ]
}
```

#### `GET /data/summary`
Get statistical summary of current data

**Response:**
```json
{
  "total_measurements": 1250,
  "active_devices": 5,
  "avg_pm25": 28.7,
  "min_pm25": 8.1,
  "max_pm25": 85.3,
  "device_list": ["sensor001", "sensor002", "sensor003"]
}
```

#### `GET /data/export`
Export historical data for analysis

**Parameters:**
- `start` (required): Start time (ISO format)
- `end` (required): End time (ISO format)
- `format` (optional): `json` or `csv` (default: json)

**Example:**
```bash
curl "http://localhost:5000/api/v1/data/export?start=2025-09-24T00:00:00Z&end=2025-09-25T00:00:00Z&format=csv"
```

**Limits:**
- Maximum export period: 30 days
- Maximum records per export: 100,000

---

### 📱 **Device Management**

#### `GET /devices`
List all active devices

**Response:**
```json
[
  {
    "device_id": "sensor001",
    "last_seen": "2025-09-25T10:25:00Z",
    "location": {
      "latitude": 13.741263,
      "longitude": 99.914530
    }
  }
]
```

#### `GET /devices/{device_id}/stats`
Get statistics for a specific device

**Parameters:**
- `hours` (optional): Time window in hours (1-168, default: 24)

**Example:**
```bash
curl "http://localhost:5000/api/v1/devices/sensor001/stats?hours=72"
```

**Response:**
```json
{
  "count": 288,
  "avg_pm25": 32.1,
  "min_pm25": 12.5,
  "max_pm25": 68.9
}
```

---

### 🚨 **Alert Management**

#### `GET /alerts`
Get all active alerts

**Response:**
```json
[
  {
    "device_id": "sensor001",
    "level": "unhealthy",
    "pm25_value": 68.5,
    "location": {
      "latitude": 13.741263,
      "longitude": 99.914530
    },
    "timestamp": "2025-09-25T10:20:00Z",
    "message": "Air quality is unhealthy. PM2.5: 68.5 μg/m³",
    "acknowledged": false
  }
]
```

#### `GET /alerts/summary`
Get alert summary statistics

**Response:**
```json
{
  "active_alerts": 2,
  "active_by_level": {
    "unhealthy": 1,
    "very_unhealthy": 1
  },
  "alerts_last_24h": 8,
  "devices_with_alerts": ["sensor001", "sensor003"],
  "last_alert_time": "2025-09-25T10:20:00Z"
}
```

#### `POST /alerts/{device_id}/acknowledge`
Acknowledge an alert for a specific device

**Example:**
```bash
curl -X POST "http://localhost:5000/api/v1/alerts/sensor001/acknowledge"
```

**Response:**
```json
{
  "message": "Alert acknowledged"
}
```

---

### ℹ️ **System Information**

#### `GET /system/info`
Get system information and capabilities

**Response:**
```json
{
  "version": "2.1.0",
  "name": "PM2.5 Ghostbuster",
  "description": "Making invisible air pollution visible",
  "philosophy": "PM2.5 is like ghost. It exists even if it is invisible.",
  "timezone": "Asia/Bangkok",
  "data_retention_hours": 720,
  "update_interval": 60,
  "features": [
    "Real-time PM2.5 monitoring",
    "GPS-tracked mobile sensors",
    "Automated alerts and notifications",
    "Historical data analysis",
    "REST API access",
    "Web-based visualization"
  ],
  "contributors": ["Werapol Bejranonda", "Baan Noorg Collaborative Arts and Culture"]
}
```

---

## 🚨 **Alert Levels**

The system uses WHO/EPA-based PM2.5 thresholds:

| Level | Range (μg/m³) | Description | Color |
|-------|---------------|-------------|-------|
| **Good** | 0 - 12 | Air quality is satisfactory | 🟢 Green |
| **Moderate** | 12.1 - 35.4 | Acceptable for most people | 🟡 Yellow |
| **Unhealthy for Sensitive** | 35.5 - 55.4 | Sensitive groups may experience minor effects | 🟠 Orange |
| **Unhealthy** | 55.5 - 150.4 | Everyone may experience health effects | 🔴 Red |
| **Very Unhealthy** | 150.5 - 250.4 | Health alert: everyone may experience serious effects | 🟣 Purple |
| **Hazardous** | 250.5+ | Emergency conditions | 🟤 Maroon |

---

## 📝 **Error Responses**

All endpoints return consistent error responses:

```json
{
  "error": "Error description",
  "message": "Detailed error message",
  "status_code": 400
}
```

**Common Status Codes:**
- `200` - Success
- `400` - Bad Request (invalid parameters)
- `404` - Not Found (resource doesn't exist)
- `500` - Internal Server Error
- `503` - Service Unavailable

---

## 🔧 **Configuration**

### API Server Settings
Add these to your `.env` file:

```env
# API Server Configuration
ENABLE_API=true
API_PORT=5000
API_HOST=0.0.0.0

# Alert System Configuration
MIN_ALERT_LEVEL=unhealthy
ENABLE_EMAIL_ALERTS=false
```

### CORS Configuration
The API includes CORS headers for web frontend integration:
- `Access-Control-Allow-Origin: *`
- `Access-Control-Allow-Methods: GET, POST, OPTIONS`
- `Access-Control-Allow-Headers: Content-Type`

---

## 🚀 **Usage Examples**

### JavaScript (Frontend)
```javascript
// Get current air quality data
async function getCurrentData() {
  const response = await fetch('http://localhost:5000/api/v1/data/current');
  const data = await response.json();
  console.log('Air quality data:', data);
}

// Check system health
async function checkHealth() {
  const response = await fetch('http://localhost:5000/api/v1/health');
  const health = await response.json();
  console.log('System status:', health.status);
}
```

### Python
```python
import requests

# Get device statistics
def get_device_stats(device_id):
    url = f"http://localhost:5000/api/v1/devices/{device_id}/stats"
    response = requests.get(url)
    return response.json()

# Acknowledge alert
def acknowledge_alert(device_id):
    url = f"http://localhost:5000/api/v1/alerts/{device_id}/acknowledge"
    response = requests.post(url)
    return response.json()
```

### cURL
```bash
# Get alert summary
curl "http://localhost:5000/api/v1/alerts/summary"

# Export data as CSV
curl "http://localhost:5000/api/v1/data/export?start=2025-09-24T00:00:00Z&end=2025-09-25T00:00:00Z&format=csv" \
  -o air_quality_data.csv
```

---

## 🔒 **Production Considerations**

### Security
- **Authentication**: Consider adding API keys or OAuth for production
- **Rate Limiting**: Implement rate limiting to prevent abuse
- **HTTPS**: Use HTTPS in production environments
- **Input Validation**: All inputs are validated server-side

### Performance
- **Caching**: API responses are cached for 1 minute
- **Database Optimization**: Queries are optimized for performance
- **Resource Monitoring**: Built-in monitoring prevents overload

### Monitoring
- **Health Checks**: Use `/health` endpoint for monitoring
- **Logging**: All API requests are logged
- **Metrics**: System metrics available via statistics endpoints

---

## 🏗 **Professional API Design**

This API implementation demonstrates professional development practices:

- **RESTful Architecture**: Clean design with proper error handling
- **Comprehensive Documentation**: Detailed examples and usage guides
- **Security Best Practices**: Input validation and secure defaults
- **Performance Optimization**: Caching and query optimization
- **Monitoring Integration**: Built-in health checks and statistics

---

**Making the invisible visible through intelligent APIs!** 👻

For more information, visit: [PM2.5 Ghostbuster GitHub Repository](https://github.com/bejranonda/pm2.5-ghostbuster)