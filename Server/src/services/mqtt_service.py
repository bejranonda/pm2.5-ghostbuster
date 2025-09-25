"""
MQTT Service for PM2.5 Ghostbuster
Handles MQTT connections and message processing
"""

import json
from typing import Callable, Optional
from threading import Event
import paho.mqtt.client as mqtt

from config.settings import config
from src.utils.logger import get_logger
from src.models.air_quality import AirQualityMeasurement


class MQTTService:
    """MQTT client service for receiving air quality data"""

    def __init__(self, message_callback: Optional[Callable[[AirQualityMeasurement], None]] = None):
        """
        Initialize MQTT service

        Args:
            message_callback: Callback function for processed messages
        """
        self.logger = get_logger('mqtt_service')
        self.client = mqtt.Client()
        self.message_callback = message_callback
        self.connected = Event()
        self._setup_client()

    def _setup_client(self) -> None:
        """Configure MQTT client callbacks and settings"""
        self.client.username_pw_set(config.MQTT_USERNAME, config.MQTT_PASSWORD)
        self.client.on_connect = self._on_connect
        self.client.on_disconnect = self._on_disconnect
        self.client.on_message = self._on_message
        self.client.on_log = self._on_log

    def _on_connect(self, client, userdata, flags, rc) -> None:
        """Handle MQTT connection"""
        if rc == 0:
            self.logger.info(f"Connected to MQTT broker at {config.MQTT_BROKER_ADDRESS}")
            self.connected.set()

            # Subscribe to all air quality topics
            topic = config.get_mqtt_topic("+", "air")
            client.subscribe(topic)
            self.logger.info(f"Subscribed to topic: {topic}")
        else:
            self.logger.error(f"Failed to connect to MQTT broker, return code: {rc}")

    def _on_disconnect(self, client, userdata, rc) -> None:
        """Handle MQTT disconnection"""
        self.logger.warning(f"Disconnected from MQTT broker, return code: {rc}")
        self.connected.clear()

    def _on_message(self, client, userdata, message) -> None:
        """
        Process incoming MQTT message

        Args:
            client: MQTT client instance
            userdata: User data
            message: MQTT message
        """
        try:
            # Extract device ID from topic
            topic_parts = message.topic.split("/")
            if len(topic_parts) < 3:
                self.logger.warning(f"Invalid topic format: {message.topic}")
                return

            device_id = topic_parts[1]
            payload_str = message.payload.decode('utf-8')

            self.logger.debug(f"Received message from {device_id}: {payload_str}")

            # Parse message into AirQualityMeasurement
            try:
                measurement = AirQualityMeasurement.from_mqtt_payload(device_id, payload_str)
                self.logger.info(f"Processed measurement: {measurement}")

                # Call the message callback if provided
                if self.message_callback:
                    self.message_callback(measurement)

            except ValueError as e:
                self.logger.error(f"Failed to parse message from {device_id}: {e}")

        except Exception as e:
            self.logger.error(f"Error processing MQTT message: {e}")

    def _on_log(self, client, userdata, level, buf) -> None:
        """Handle MQTT client logs"""
        if config.DEBUG:
            self.logger.debug(f"MQTT: {buf}")

    def connect(self, timeout: int = 30) -> bool:
        """
        Connect to MQTT broker

        Args:
            timeout: Connection timeout in seconds

        Returns:
            True if connected successfully
        """
        try:
            self.logger.info(f"Connecting to MQTT broker: {config.MQTT_BROKER_ADDRESS}:{config.MQTT_BROKER_PORT}")
            self.client.connect(config.MQTT_BROKER_ADDRESS, config.MQTT_BROKER_PORT, keepalive=60)

            # Start the network loop
            self.client.loop_start()

            # Wait for connection
            if self.connected.wait(timeout):
                return True
            else:
                self.logger.error("Connection timeout")
                return False

        except Exception as e:
            self.logger.error(f"Failed to connect to MQTT broker: {e}")
            return False

    def disconnect(self) -> None:
        """Disconnect from MQTT broker"""
        self.logger.info("Disconnecting from MQTT broker")
        self.client.loop_stop()
        self.client.disconnect()

    def is_connected(self) -> bool:
        """Check if connected to MQTT broker"""
        return self.connected.is_set()

    def wait_for_messages(self) -> None:
        """Keep the service running to receive messages"""
        try:
            self.client.loop_forever()
        except KeyboardInterrupt:
            self.logger.info("Received interrupt signal")
        finally:
            self.disconnect()