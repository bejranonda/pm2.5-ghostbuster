"""
PM2.5 Ghostbuster Configuration Management
Secure configuration loading with environment variables
"""

import os
import logging
from pathlib import Path
from typing import Optional
from dotenv import load_dotenv

# Load environment variables from .env file
env_path = Path(__file__).parent / '.env'
load_dotenv(env_path)

class Config:
    """Application configuration with secure defaults"""

    # MQTT Configuration
    MQTT_BROKER_ADDRESS: str = os.getenv('MQTT_BROKER_ADDRESS', 'mqtt.thalay.eu')
    MQTT_BROKER_PORT: int = int(os.getenv('MQTT_BROKER_PORT', '1883'))
    MQTT_USERNAME: str = os.getenv('MQTT_USERNAME', 'pm25')
    MQTT_PASSWORD: Optional[str] = os.getenv('MQTT_PASSWORD')
    MQTT_TOPIC_PREFIX: str = os.getenv('MQTT_TOPIC_PREFIX', 'pm25')

    # InfluxDB Configuration
    INFLUX_HOST: str = os.getenv('INFLUX_HOST', 'localhost')
    INFLUX_PORT: int = int(os.getenv('INFLUX_PORT', '8086'))
    INFLUX_DATABASE: str = os.getenv('INFLUX_DATABASE', 'pm25gps')
    INFLUX_USERNAME: Optional[str] = os.getenv('INFLUX_USERNAME')
    INFLUX_PASSWORD: Optional[str] = os.getenv('INFLUX_PASSWORD')

    # File Paths
    GEOJSON_OUTPUT_PATH: str = os.getenv('GEOJSON_OUTPUT_PATH', '/var/www/html/gj/pm25gps.geojson')
    CSV_OUTPUT_PATH: str = os.getenv('CSV_OUTPUT_PATH', '/var/www/html/csv/fire.csv')
    LOG_PATH: str = os.getenv('LOG_PATH', '/var/log/pm25/')

    # Data Processing
    DATA_RETENTION_HOURS: int = int(os.getenv('DATA_RETENTION_HOURS', '720'))
    GEOJSON_UPDATE_INTERVAL: int = int(os.getenv('GEOJSON_UPDATE_INTERVAL', '60'))
    CSV_UPDATE_INTERVAL: int = int(os.getenv('CSV_UPDATE_INTERVAL', '300'))

    # Timezone
    DEFAULT_TIMEZONE: str = os.getenv('DEFAULT_TIMEZONE', 'Asia/Bangkok')

    # NASA FIRMS API
    NASA_FIRMS_API_KEY: Optional[str] = os.getenv('NASA_FIRMS_API_KEY')
    FIRMS_MAP_KEY: Optional[str] = os.getenv('FIRMS_MAP_KEY')

    # Application Settings
    DEBUG: bool = os.getenv('DEBUG', 'false').lower() == 'true'
    LOG_LEVEL: str = os.getenv('LOG_LEVEL', 'INFO')

    @classmethod
    def validate(cls) -> bool:
        """Validate that required configuration is present"""
        required_fields = [
            'MQTT_PASSWORD',
            'INFLUX_DATABASE'
        ]

        missing_fields = []
        for field in required_fields:
            if not getattr(cls, field):
                missing_fields.append(field)

        if missing_fields:
            logging.error(f"Missing required configuration: {', '.join(missing_fields)}")
            return False

        return True

    @classmethod
    def get_mqtt_topic(cls, device_id: str, topic_type: str) -> str:
        """Generate MQTT topic for device"""
        return f"{cls.MQTT_TOPIC_PREFIX}/{device_id}/{topic_type}"

    @classmethod
    def ensure_directories(cls) -> None:
        """Ensure required directories exist"""
        directories = [
            os.path.dirname(cls.GEOJSON_OUTPUT_PATH),
            os.path.dirname(cls.CSV_OUTPUT_PATH),
            cls.LOG_PATH
        ]

        for directory in directories:
            os.makedirs(directory, exist_ok=True)

# Global configuration instance
config = Config()