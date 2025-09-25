"""
GeoJSON Service for PM2.5 Ghostbuster
Generates GeoJSON files from air quality data
"""

import json
from pathlib import Path
from typing import List, Dict, Any, Optional
import geojson

from config.settings import config
from src.utils.logger import get_logger
from src.utils.timezone_utils import tz_manager
from src.services.influx_service import InfluxService


class GeoJSONService:
    """Service for generating GeoJSON files from air quality data"""

    def __init__(self, influx_service: Optional[InfluxService] = None):
        """
        Initialize GeoJSON service

        Args:
            influx_service: InfluxDB service instance
        """
        self.logger = get_logger('geojson_service')
        self.influx_service = influx_service or InfluxService()

    def _data_point_to_feature(self, point: Dict[str, Any]) -> Dict[str, Any]:
        """
        Convert InfluxDB data point to GeoJSON feature

        Args:
            point: InfluxDB data point

        Returns:
            GeoJSON feature dictionary
        """
        # Convert UTC time to Bangkok time for display
        utc_time = point.get('time')
        if utc_time:
            try:
                local_time = tz_manager.utc_to_local(utc_time)
                formatted_time = local_time.strftime('%Y-%m-%d %H:%M:%S')
            except Exception as e:
                self.logger.warning(f"Time conversion error: {e}")
                formatted_time = str(utc_time)
        else:
            formatted_time = "Unknown"

        properties = {
            "device_id": point.get("device_id", "unknown"),
            "pm25": round(float(point.get("pm25", 0)), 2),
            "time": formatted_time
        }

        # Add speed if available
        if point.get("speed"):
            properties["speed"] = round(float(point["speed"]), 1)

        # Add any additional fields
        for key, value in point.items():
            if key not in ["device_id", "pm25", "latitude", "longitude", "time", "speed"]:
                properties[key] = value

        return {
            "type": "Feature",
            "geometry": {
                "type": "Point",
                "coordinates": [
                    round(float(point.get("longitude", 0)), 6),
                    round(float(point.get("latitude", 0)), 6)
                ]
            },
            "properties": properties
        }

    def generate_geojson(self, hours: Optional[int] = None) -> Optional[Dict[str, Any]]:
        """
        Generate GeoJSON from recent air quality data

        Args:
            hours: Hours of data to include (defaults to config value)

        Returns:
            GeoJSON FeatureCollection or None if error
        """
        hours = hours or config.DATA_RETENTION_HOURS

        try:
            # Get data from InfluxDB
            data_points = self.influx_service.query_recent_data(hours)

            if data_points is None:
                self.logger.error("Failed to retrieve data from InfluxDB")
                return None

            if not data_points:
                self.logger.warning("No data points found")
                return geojson.FeatureCollection([])

            # Convert to GeoJSON features
            features = []
            for point in data_points:
                try:
                    feature = self._data_point_to_feature(point)
                    features.append(feature)
                except Exception as e:
                    self.logger.warning(f"Failed to convert point to feature: {e}")
                    continue

            # Create FeatureCollection
            feature_collection = geojson.FeatureCollection(features)

            self.logger.info(f"Generated GeoJSON with {len(features)} features from last {hours} hours")
            return feature_collection

        except Exception as e:
            self.logger.error(f"Failed to generate GeoJSON: {e}")
            return None

    def save_geojson_file(self, geojson_data: Optional[Dict[str, Any]] = None,
                         file_path: Optional[str] = None) -> bool:
        """
        Save GeoJSON data to file

        Args:
            geojson_data: GeoJSON data (generates new if None)
            file_path: Output file path (uses config default if None)

        Returns:
            True if successful
        """
        file_path = file_path or config.GEOJSON_OUTPUT_PATH

        try:
            # Generate GeoJSON data if not provided
            if geojson_data is None:
                geojson_data = self.generate_geojson()

            if geojson_data is None:
                self.logger.error("No GeoJSON data to save")
                return False

            # Ensure output directory exists
            output_path = Path(file_path)
            output_path.parent.mkdir(parents=True, exist_ok=True)

            # Write to file
            with open(file_path, 'w', encoding='utf-8') as f:
                json.dump(geojson_data, f, ensure_ascii=False, separators=(',', ':'))

            feature_count = len(geojson_data.get('features', []))
            self.logger.info(f"Saved GeoJSON file with {feature_count} features to: {file_path}")
            return True

        except Exception as e:
            self.logger.error(f"Failed to save GeoJSON file: {e}")
            return False

    def get_summary_stats(self) -> Optional[Dict[str, Any]]:
        """
        Get summary statistics for current data

        Returns:
            Summary statistics or None if error
        """
        try:
            data_points = self.influx_service.query_recent_data(24)  # Last 24 hours

            if not data_points:
                return None

            # Calculate statistics
            pm25_values = [float(point.get('pm25', 0)) for point in data_points if point.get('pm25')]
            device_ids = set(point.get('device_id') for point in data_points if point.get('device_id'))

            if not pm25_values:
                return None

            stats = {
                'total_measurements': len(data_points),
                'active_devices': len(device_ids),
                'avg_pm25': round(sum(pm25_values) / len(pm25_values), 2),
                'min_pm25': round(min(pm25_values), 2),
                'max_pm25': round(max(pm25_values), 2),
                'device_list': list(device_ids)
            }

            return stats

        except Exception as e:
            self.logger.error(f"Failed to generate summary stats: {e}")
            return None