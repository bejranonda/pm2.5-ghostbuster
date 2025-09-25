"""
PM2.5 Ghostbuster - MQTT Logger
Refactored version of PM25mqttLog.py with better structure
"""

import sys
from pathlib import Path

# Add project root to Python path
project_root = Path(__file__).parent.parent
sys.path.insert(0, str(project_root))

from config.settings import config
from src.utils.logger import get_logger
from src.utils.timezone_utils import tz_manager
from src.services.mqtt_service import MQTTService
from src.models.air_quality import AirQualityMeasurement


class MQTTLogger:
    """MQTT message logger for debugging and monitoring"""

    def __init__(self):
        """Initialize the MQTT logger"""
        self.logger = get_logger('mqtt_logger')

    def _log_measurement(self, measurement: AirQualityMeasurement) -> None:
        """
        Log measurement details

        Args:
            measurement: Air quality measurement from MQTT
        """
        # Format timestamp in Bangkok timezone
        local_time = tz_manager.format_local_time(measurement.timestamp)

        self.logger.info(f"#BKK: {tz_manager.now_local().strftime('%Y-%m-%d %H:%M:%S')}")
        self.logger.info(f"Device: {measurement.device_id}")
        self.logger.info(f"PM2.5: {measurement.pm25} μg/m³")
        self.logger.info(f"Location: ({measurement.latitude}, {measurement.longitude})")
        self.logger.info(f"Timestamp: {local_time}")

        if measurement.speed is not None:
            self.logger.info(f"Speed: {measurement.speed} km/h")

        self.logger.info("-" * 50)

    def start(self) -> None:
        """Start the MQTT logger"""
        self.logger.info("Starting MQTT Logger")

        # Validate configuration
        if not config.validate():
            self.logger.error("Invalid configuration. Please check your .env file.")
            return

        # Create MQTT service with logging callback
        mqtt_service = MQTTService(self._log_measurement)

        # Connect to MQTT broker
        if not mqtt_service.connect():
            self.logger.error("Failed to connect to MQTT broker")
            return

        try:
            self.logger.info("MQTT Logger started successfully")
            self.logger.info(f"Listening for messages on: {config.get_mqtt_topic('#', '')}")

            # Keep receiving messages
            mqtt_service.wait_for_messages()

        except KeyboardInterrupt:
            self.logger.info("Received shutdown signal")
        finally:
            mqtt_service.disconnect()


def main():
    """Main entry point"""
    logger = MQTTLogger()

    try:
        logger.start()
    except Exception as e:
        print(f"Fatal error: {e}")
        sys.exit(1)


if __name__ == "__main__":
    main()