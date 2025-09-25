"""
PM2.5 Ghostbuster - Main Data Collector
Enhanced version with alerts, API, and advanced monitoring

Enhanced for v2.1.0 by Claude Code Assistant
"""

import sys
import time
import threading
from pathlib import Path

# Add project root to Python path
project_root = Path(__file__).parent.parent
sys.path.insert(0, str(project_root))

from config.settings import config
from src.utils.logger import get_logger
from src.services.mqtt_service import MQTTService
from src.services.influx_service import InfluxService
from src.services.geojson_service import GeoJSONService
from src.services.alert_service import AlertService
from src.services.api_service import APIService
from src.models.air_quality import AirQualityMeasurement


class PM25DataCollector:
    """Enhanced data collector service for PM2.5 Ghostbuster with alerts and API"""

    def __init__(self):
        """Initialize the enhanced data collector"""
        self.logger = get_logger('data_collector')

        # Initialize services
        self.influx_service = InfluxService()
        self.geojson_service = GeoJSONService(self.influx_service)
        self.alert_service = AlertService()
        self.api_service = APIService(self.influx_service, self.geojson_service, self.alert_service)
        self.mqtt_service = MQTTService(self._process_measurement)

        self.running = False
        self.stats = {
            'messages_processed': 0,
            'alerts_generated': 0,
            'start_time': time.time(),
            'last_measurement_time': None
        }

        # Setup alert notifications
        self.alert_service.add_notification_callback(self._handle_alert_notification)

    def _process_measurement(self, measurement: AirQualityMeasurement) -> None:
        """
        Enhanced measurement processing with alerts and statistics

        Args:
            measurement: Air quality measurement from MQTT
        """
        try:
            # Update statistics
            self.stats['messages_processed'] += 1
            self.stats['last_measurement_time'] = time.time()

            # Store in InfluxDB
            success = self.influx_service.write_measurement(measurement)

            if success:
                self.logger.info(f"Stored measurement: {measurement}")

                # Process for alerts
                alert = self.alert_service.process_measurement(measurement)
                if alert:
                    self.stats['alerts_generated'] += 1
                    self.logger.warning(f"Alert generated: {alert.message}")

            else:
                self.logger.error(f"Failed to store measurement: {measurement}")

        except Exception as e:
            self.logger.error(f"Error processing measurement: {e}")

    def _handle_alert_notification(self, alert) -> None:
        """
        Handle alert notifications

        Args:
            alert: Alert instance
        """
        self.logger.warning(f"NOTIFICATION: {alert.level.value.upper()} alert for device {alert.device_id}")
        # Additional notification logic can be added here (e.g., webhook calls)

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

    def _log_statistics_periodically(self) -> None:
        """Log system statistics periodically"""
        while self.running:
            try:
                time.sleep(300)  # Log every 5 minutes

                uptime = time.time() - self.stats['start_time']
                uptime_hours = uptime / 3600

                self.logger.info(f"STATS: Uptime: {uptime_hours:.1f}h, "
                               f"Messages: {self.stats['messages_processed']}, "
                               f"Alerts: {self.stats['alerts_generated']}, "
                               f"Active Alerts: {len(self.alert_service.get_active_alerts())}")

                # Log alert summary
                alert_summary = self.alert_service.get_alert_summary()
                if alert_summary['active_alerts'] > 0:
                    self.logger.info(f"Alert Summary: {alert_summary}")

            except Exception as e:
                self.logger.error(f"Error in statistics logging: {e}")
                time.sleep(60)  # Wait longer on error

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
            # Start background threads
            geojson_thread = threading.Thread(target=self._generate_geojson_periodically, daemon=True)
            geojson_thread.start()

            # Start API server in background if enabled
            api_enabled = getattr(config, 'ENABLE_API', True)
            if api_enabled:
                api_port = getattr(config, 'API_PORT', 5000)
                api_thread = threading.Thread(
                    target=lambda: self.api_service.run(port=api_port, debug=config.DEBUG),
                    daemon=True
                )
                api_thread.start()
                self.logger.info(f"API server started on port {api_port}")

            # Start statistics logging thread
            stats_thread = threading.Thread(target=self._log_statistics_periodically, daemon=True)
            stats_thread.start()

            self.logger.info("Enhanced data collector started successfully")
            self.logger.info(f"Listening for MQTT messages on topic: {config.get_mqtt_topic('+', 'air')}")
            self.logger.info("Features: Data Collection, Alerts, REST API, Statistics")

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