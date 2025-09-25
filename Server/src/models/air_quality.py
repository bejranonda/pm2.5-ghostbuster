"""
Air Quality Data Models
Defines data structures for PM2.5 measurements
"""

from datetime import datetime
from dataclasses import dataclass
from typing import Optional, Dict, Any
import json


@dataclass
class AirQualityMeasurement:
    """Represents a single PM2.5 measurement with location data"""

    device_id: str
    pm25: float
    latitude: float
    longitude: float
    timestamp: datetime
    speed: Optional[float] = None
    additional_data: Optional[Dict[str, Any]] = None

    def to_influx_point(self) -> Dict[str, Any]:
        """
        Convert to InfluxDB point format

        Returns:
            InfluxDB point dictionary
        """
        fields = {
            "pm25": float(self.pm25),
            "latitude": float(self.latitude),
            "longitude": float(self.longitude)
        }

        if self.speed is not None:
            fields["speed"] = float(self.speed)

        if self.additional_data:
            fields.update(self.additional_data)

        return {
            "measurement": "air_quality",
            "tags": {
                "device_id": self.device_id
            },
            "time": self.timestamp,
            "fields": fields
        }

    def to_geojson_feature(self) -> Dict[str, Any]:
        """
        Convert to GeoJSON feature format

        Returns:
            GeoJSON feature dictionary
        """
        properties = {
            "device_id": self.device_id,
            "pm25": self.pm25,
            "time": self.timestamp.isoformat()
        }

        if self.speed is not None:
            properties["speed"] = self.speed

        if self.additional_data:
            properties.update(self.additional_data)

        return {
            "type": "Feature",
            "geometry": {
                "type": "Point",
                "coordinates": [self.longitude, self.latitude]
            },
            "properties": properties
        }

    @classmethod
    def from_mqtt_payload(cls, device_id: str, payload: str) -> 'AirQualityMeasurement':
        """
        Create measurement from MQTT payload

        Args:
            device_id: Device identifier
            payload: JSON payload string

        Returns:
            AirQualityMeasurement instance

        Raises:
            ValueError: If payload is invalid
        """
        try:
            data = json.loads(payload)

            # Handle different timestamp formats
            if 'tst' in data:
                timestamp = datetime.utcfromtimestamp(data['tst'])
            elif 'timestamp' in data:
                timestamp = datetime.fromisoformat(data['timestamp'])
            else:
                timestamp = datetime.utcnow()

            return cls(
                device_id=device_id,
                pm25=float(data['pm25']),
                latitude=float(data['lat']),
                longitude=float(data['lon']),
                timestamp=timestamp,
                speed=float(data.get('speed')) if 'speed' in data else None,
                additional_data={k: v for k, v in data.items()
                               if k not in ['pm25', 'lat', 'lon', 'tst', 'timestamp', 'speed']}
            )

        except (json.JSONDecodeError, KeyError, ValueError) as e:
            raise ValueError(f"Invalid MQTT payload: {e}")

    def __str__(self) -> str:
        return f"PM2.5: {self.pm25} μg/m³ at ({self.latitude}, {self.longitude}) from {self.device_id}"