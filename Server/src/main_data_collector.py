"""
PM2.5 Ghostbuster - Main Data Collector
Refactored version of PM25ikeav2.py with proper error handling and logging
"""

import sys
import time
from pathlib import Path

# Add project root to Python path
project_root = Path(__file__).parent.parent
sys.path.insert(0, str(project_root))

from config.settings import config
from src.utils.logger import get_logger
from src.services.mqtt_service import MQTTService
from src.services.influx_service import InfluxService
from src.services.geojson_service import GeoJSONService
from src.models.air_quality import AirQualityMeasurement


class PM25DataCollector:
    """Main data collector service for PM2.5 Ghostbuster"""

    def __init__(self):
        """Initialize the data collector"""
        self.logger = get_logger('data_collector')
        self.influx_service = InfluxService()
        self.geojson_service = GeoJSONService(self.influx_service)
        self.mqtt_service = MQTTService(self._process_measurement)
        self.running = False

    def _process_measurement(self, measurement: AirQualityMeasurement) -> None:
        """
        Process incoming air quality measurement

        Args:
            measurement: Air quality measurement from MQTT
        """
        try:
            # Store in InfluxDB
            success = self.influx_service.write_measurement(measurement)

            if success:
                self.logger.info(f"Stored measurement: {measurement}")
            else:
                self.logger.error(f"Failed to store measurement: {measurement}")

        except Exception as e:
            self.logger.error(f"Error processing measurement: {e}")

    def _generate_geojson_periodically(self) -> None:
        """Generate GeoJSON files periodically"""
        last_update = 0

        while self.running:
            try:
                current_time = time.time()

                # Check if it's time to update GeoJSON
                if current_time - last_update >= config.GEOJSON_UPDATE_INTERVAL:
                    self.logger.info("Generating GeoJSON file...")

                    success = self.geojson_service.save_geojson_file()

                    if success:
                        self.logger.info("GeoJSON file updated successfully")
                    else:
                        self.logger.error("Failed to update GeoJSON file")

                    last_update = current_time

                time.sleep(10)  # Check every 10 seconds

            except Exception as e:
                self.logger.error(f"Error in GeoJSON generation loop: {e}")
                time.sleep(30)  # Wait longer on error

    def start(self) -> None:
        """Start the data collector service"""
        self.logger.info("Starting PM2.5 Data Collector")

        # Validate configuration
        if not config.validate():
            self.logger.error("Invalid configuration. Please check your .env file.")
            return

        # Ensure required directories exist
        config.ensure_directories()

        # Connect to InfluxDB
        if not self.influx_service.is_connected():
            self.logger.error("Failed to connect to InfluxDB")
            return

        # Connect to MQTT broker
        if not self.mqtt_service.connect():
            self.logger.error("Failed to connect to MQTT broker")
            return

        self.running = True

        try:
            # Start GeoJSON generation in background
            import threading
            geojson_thread = threading.Thread(target=self._generate_geojson_periodically, daemon=True)
            geojson_thread.start()

            self.logger.info("Data collector started successfully")
            self.logger.info(f"Listening for MQTT messages on topic: {config.get_mqtt_topic('+', 'air')}")

            # Keep the main thread alive and handle MQTT messages
            self.mqtt_service.wait_for_messages()

        except KeyboardInterrupt:
            self.logger.info("Received shutdown signal")
        except Exception as e:
            self.logger.error(f"Unexpected error: {e}")
        finally:
            self.stop()

    def stop(self) -> None:
        """Stop the data collector service"""
        self.logger.info("Stopping PM2.5 Data Collector")
        self.running = False

        if self.mqtt_service:
            self.mqtt_service.disconnect()

    def health_check(self) -> bool:
        """
        Perform health check

        Returns:
            True if all services are healthy
        """
        checks = {
            'mqtt_connected': self.mqtt_service.is_connected() if self.mqtt_service else False,
            'influx_connected': self.influx_service.is_connected() if self.influx_service else False
        }

        all_healthy = all(checks.values())

        self.logger.info(f"Health check: {checks}")
        return all_healthy


def main():
    """Main entry point"""
    collector = PM25DataCollector()

    try:
        collector.start()
    except Exception as e:
        print(f"Fatal error: {e}")
        sys.exit(1)


if __name__ == "__main__":
    main()