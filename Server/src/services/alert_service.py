"""
PM2.5 Ghostbuster - Alert Service
Handles threshold-based alerts and notifications

Enhanced for v2.1.0 by Claude Code Assistant
"""

import json
from datetime import datetime, timedelta
from typing import Dict, List, Optional, Callable
from dataclasses import dataclass, asdict
from enum import Enum
import smtplib
from email.mime.text import MIMEText
from email.mime.multipart import MIMEMultipart

from config.settings import config
from src.utils.logger import get_logger
from src.utils.timezone_utils import tz_manager
from src.models.air_quality import AirQualityMeasurement


class AlertLevel(Enum):
    """PM2.5 alert levels based on AQI standards"""
    GOOD = "good"
    MODERATE = "moderate"
    UNHEALTHY_SENSITIVE = "unhealthy_for_sensitive"
    UNHEALTHY = "unhealthy"
    VERY_UNHEALTHY = "very_unhealthy"
    HAZARDOUS = "hazardous"


@dataclass
class AlertThreshold:
    """Alert threshold configuration"""
    level: AlertLevel
    min_pm25: float
    max_pm25: float
    color: str
    message: str


@dataclass
class Alert:
    """Alert instance"""
    device_id: str
    level: AlertLevel
    pm25_value: float
    location: Dict[str, float]
    timestamp: datetime
    message: str
    acknowledged: bool = False


class AlertService:
    """Service for managing PM2.5 alerts and notifications"""

    # WHO/EPA PM2.5 thresholds (μg/m³)
    THRESHOLDS = [
        AlertThreshold(AlertLevel.GOOD, 0, 12, "#00e400", "Air quality is good"),
        AlertThreshold(AlertLevel.MODERATE, 12.1, 35.4, "#ffff00", "Air quality is moderate"),
        AlertThreshold(AlertLevel.UNHEALTHY_SENSITIVE, 35.5, 55.4, "#ff7e00",
                      "Unhealthy for sensitive groups"),
        AlertThreshold(AlertLevel.UNHEALTHY, 55.5, 150.4, "#ff0000", "Air quality is unhealthy"),
        AlertThreshold(AlertLevel.VERY_UNHEALTHY, 150.5, 250.4, "#8f3f97",
                      "Air quality is very unhealthy"),
        AlertThreshold(AlertLevel.HAZARDOUS, 250.5, float('inf'), "#7e0023",
                      "Air quality is hazardous")
    ]

    def __init__(self):
        """Initialize the alert service"""
        self.logger = get_logger('alert_service')
        self.active_alerts: Dict[str, Alert] = {}
        self.alert_history: List[Alert] = []
        self.notification_callbacks: List[Callable[[Alert], None]] = []

        # Load alert configuration
        self.email_enabled = bool(config.SMTP_HOST if hasattr(config, 'SMTP_HOST') else None)
        self.min_alert_level = getattr(config, 'MIN_ALERT_LEVEL', AlertLevel.UNHEALTHY.value)

    def add_notification_callback(self, callback: Callable[[Alert], None]) -> None:
        """Add a notification callback function"""
        self.notification_callbacks.append(callback)

    def get_alert_level(self, pm25_value: float) -> AlertThreshold:
        """
        Determine alert level for PM2.5 value

        Args:
            pm25_value: PM2.5 concentration in μg/m³

        Returns:
            AlertThreshold for the PM2.5 level
        """
        for threshold in self.THRESHOLDS:
            if threshold.min_pm25 <= pm25_value <= threshold.max_pm25:
                return threshold

        # Default to hazardous for very high values
        return self.THRESHOLDS[-1]

    def process_measurement(self, measurement: AirQualityMeasurement) -> Optional[Alert]:
        """
        Process measurement and generate alerts if necessary

        Args:
            measurement: Air quality measurement

        Returns:
            Alert if threshold exceeded, None otherwise
        """
        try:
            threshold = self.get_alert_level(measurement.pm25)

            # Check if we should trigger an alert
            should_alert = self._should_trigger_alert(measurement.device_id, threshold)

            if should_alert:
                alert = Alert(
                    device_id=measurement.device_id,
                    level=threshold.level,
                    pm25_value=measurement.pm25,
                    location={
                        'latitude': measurement.latitude,
                        'longitude': measurement.longitude
                    },
                    timestamp=measurement.timestamp,
                    message=f"{threshold.message}. PM2.5: {measurement.pm25} μg/m³"
                )

                self._trigger_alert(alert)
                return alert

            # Clear alert if conditions improved
            self._clear_alert_if_improved(measurement.device_id, threshold)
            return None

        except Exception as e:
            self.logger.error(f"Error processing measurement for alerts: {e}")
            return None

    def _should_trigger_alert(self, device_id: str, threshold: AlertThreshold) -> bool:
        """
        Determine if an alert should be triggered

        Args:
            device_id: Device identifier
            threshold: Alert threshold

        Returns:
            True if alert should be triggered
        """
        min_level_index = next(i for i, t in enumerate(self.THRESHOLDS)
                              if t.level.value == self.min_alert_level)
        current_level_index = next(i for i, t in enumerate(self.THRESHOLDS)
                                  if t.level == threshold.level)

        # Only alert if above minimum level
        if current_level_index < min_level_index:
            return False

        # Check if this is a new alert or escalation
        existing_alert = self.active_alerts.get(device_id)
        if not existing_alert:
            return True  # New alert

        existing_level_index = next(i for i, t in enumerate(self.THRESHOLDS)
                                   if t.level == existing_alert.level)

        # Alert if escalation (higher severity)
        return current_level_index > existing_level_index

    def _clear_alert_if_improved(self, device_id: str, threshold: AlertThreshold) -> None:
        """Clear active alert if conditions have improved"""
        existing_alert = self.active_alerts.get(device_id)
        if not existing_alert:
            return

        # If current level is better than alert level, clear the alert
        existing_level_index = next(i for i, t in enumerate(self.THRESHOLDS)
                                   if t.level == existing_alert.level)
        current_level_index = next(i for i, t in enumerate(self.THRESHOLDS)
                                  if t.level == threshold.level)

        if current_level_index < existing_level_index:
            self.logger.info(f"Clearing alert for device {device_id} - conditions improved")
            del self.active_alerts[device_id]

    def _trigger_alert(self, alert: Alert) -> None:
        """
        Trigger an alert and send notifications

        Args:
            alert: Alert to trigger
        """
        self.logger.warning(f"ALERT TRIGGERED: {alert.message} for device {alert.device_id}")

        # Store active alert
        self.active_alerts[alert.device_id] = alert

        # Add to history
        self.alert_history.append(alert)

        # Keep history manageable (last 1000 alerts)
        if len(self.alert_history) > 1000:
            self.alert_history = self.alert_history[-1000:]

        # Send notifications
        self._send_notifications(alert)

    def _send_notifications(self, alert: Alert) -> None:
        """
        Send alert notifications via configured channels

        Args:
            alert: Alert to notify about
        """
        # Call registered callbacks
        for callback in self.notification_callbacks:
            try:
                callback(alert)
            except Exception as e:
                self.logger.error(f"Error in notification callback: {e}")

        # Send email if configured
        if self.email_enabled:
            self._send_email_alert(alert)

    def _send_email_alert(self, alert: Alert) -> None:
        """
        Send email alert (if configured)

        Args:
            alert: Alert to send via email
        """
        try:
            if not hasattr(config, 'SMTP_HOST'):
                return

            msg = MIMEMultipart()
            msg['From'] = getattr(config, 'SMTP_FROM', 'pm25-ghostbuster@thalay.eu')
            msg['To'] = getattr(config, 'ALERT_EMAIL', '')
            msg['Subject'] = f"PM2.5 Alert: {alert.level.value.upper()} - Device {alert.device_id}"

            body = f"""
PM2.5 Ghostbuster Alert

Device: {alert.device_id}
Alert Level: {alert.level.value.upper()}
PM2.5 Value: {alert.pm25_value} μg/m³
Location: {alert.location['latitude']:.6f}, {alert.location['longitude']:.6f}
Time: {tz_manager.format_local_time(alert.timestamp)}

Message: {alert.message}

View live data: {getattr(config, 'MAP_URL', 'https://map.thalay.eu')}

---
PM2.5 Ghostbuster Alert System
"PM2.5 is like ghost. It exists even if it is invisible."
            """

            msg.attach(MIMEText(body, 'plain'))

            server = smtplib.SMTP(config.SMTP_HOST, getattr(config, 'SMTP_PORT', 587))
            server.starttls()
            if hasattr(config, 'SMTP_USERNAME'):
                server.login(config.SMTP_USERNAME, config.SMTP_PASSWORD)

            server.send_message(msg)
            server.quit()

            self.logger.info(f"Email alert sent for device {alert.device_id}")

        except Exception as e:
            self.logger.error(f"Failed to send email alert: {e}")

    def get_active_alerts(self) -> List[Alert]:
        """Get list of active alerts"""
        return list(self.active_alerts.values())

    def get_alert_history(self, hours: int = 24) -> List[Alert]:
        """
        Get alert history for specified time period

        Args:
            hours: Hours of history to retrieve

        Returns:
            List of alerts within time period
        """
        cutoff_time = datetime.utcnow() - timedelta(hours=hours)
        return [alert for alert in self.alert_history if alert.timestamp >= cutoff_time]

    def acknowledge_alert(self, device_id: str) -> bool:
        """
        Acknowledge an active alert

        Args:
            device_id: Device with alert to acknowledge

        Returns:
            True if alert was acknowledged
        """
        alert = self.active_alerts.get(device_id)
        if alert:
            alert.acknowledged = True
            self.logger.info(f"Alert acknowledged for device {device_id}")
            return True
        return False

    def get_alert_summary(self) -> Dict:
        """
        Get summary of current alert status

        Returns:
            Dictionary with alert statistics
        """
        active_by_level = {}
        for alert in self.active_alerts.values():
            level = alert.level.value
            active_by_level[level] = active_by_level.get(level, 0) + 1

        recent_alerts = self.get_alert_history(24)

        return {
            'active_alerts': len(self.active_alerts),
            'active_by_level': active_by_level,
            'alerts_last_24h': len(recent_alerts),
            'devices_with_alerts': list(self.active_alerts.keys()),
            'last_alert_time': max([a.timestamp for a in recent_alerts], default=None)
        }

    def export_alerts(self, start_time: datetime, end_time: datetime) -> List[Dict]:
        """
        Export alerts for specified time period

        Args:
            start_time: Start of time period
            end_time: End of time period

        Returns:
            List of alert dictionaries
        """
        alerts_in_period = [
            alert for alert in self.alert_history
            if start_time <= alert.timestamp <= end_time
        ]

        return [asdict(alert) for alert in alerts_in_period]