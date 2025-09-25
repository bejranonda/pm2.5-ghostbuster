"""
InfluxDB Service for PM2.5 Ghostbuster
Handles database operations for air quality data
"""

from datetime import datetime, timedelta
from typing import List, Optional, Dict, Any
from influxdb import InfluxDBClient
from influxdb.exceptions import InfluxDBError

from config.settings import config
from src.utils.logger import get_logger
from src.models.air_quality import AirQualityMeasurement


class InfluxService:
    """Service for interacting with InfluxDB"""

    def __init__(self):
        """Initialize InfluxDB service"""
        self.logger = get_logger('influx_service')
        self.client = None
        self._connect()

    def _connect(self) -> None:
        """Establish connection to InfluxDB"""
        try:
            self.client = InfluxDBClient(
                host=config.INFLUX_HOST,
                port=config.INFLUX_PORT,
                username=config.INFLUX_USERNAME,
                password=config.INFLUX_PASSWORD,
                database=config.INFLUX_DATABASE,
                timeout=30,
                retries=3
            )

            # Test connection
            self.client.ping()
            self.logger.info(f"Connected to InfluxDB at {config.INFLUX_HOST}:{config.INFLUX_PORT}")

            # Ensure database exists
            self._ensure_database()

        except Exception as e:
            self.logger.error(f"Failed to connect to InfluxDB: {e}")
            self.client = None

    def _ensure_database(self) -> None:
        """Ensure the database exists"""
        if not self.client:
            return

        try:
            databases = self.client.get_list_database()
            db_names = [db['name'] for db in databases]

            if config.INFLUX_DATABASE not in db_names:
                self.client.create_database(config.INFLUX_DATABASE)
                self.logger.info(f"Created database: {config.INFLUX_DATABASE}")

        except InfluxDBError as e:
            self.logger.error(f"Failed to ensure database exists: {e}")

    def is_connected(self) -> bool:
        """Check if connected to InfluxDB"""
        if not self.client:
            return False

        try:
            self.client.ping()
            return True
        except:
            return False

    def write_measurement(self, measurement: AirQualityMeasurement) -> bool:
        """
        Write single measurement to InfluxDB

        Args:
            measurement: Air quality measurement to store

        Returns:
            True if successful
        """
        if not self.client:
            self.logger.error("InfluxDB client not connected")
            return False

        try:
            point = measurement.to_influx_point()
            result = self.client.write_points([point])

            if result:
                self.logger.debug(f"Wrote measurement to InfluxDB: {measurement.device_id}")
                return True
            else:
                self.logger.error("Failed to write measurement to InfluxDB")
                return False

        except InfluxDBError as e:
            self.logger.error(f"InfluxDB write error: {e}")
            return False

    def write_measurements_batch(self, measurements: List[AirQualityMeasurement]) -> bool:
        """
        Write multiple measurements to InfluxDB in batch

        Args:
            measurements: List of measurements to store

        Returns:
            True if successful
        """
        if not self.client:
            self.logger.error("InfluxDB client not connected")
            return False

        if not measurements:
            return True

        try:
            points = [m.to_influx_point() for m in measurements]
            result = self.client.write_points(points)

            if result:
                self.logger.info(f"Wrote {len(measurements)} measurements to InfluxDB")
                return True
            else:
                self.logger.error("Failed to write measurements batch to InfluxDB")
                return False

        except InfluxDBError as e:
            self.logger.error(f"InfluxDB batch write error: {e}")
            return False

    def query_recent_data(self, hours: int = None) -> Optional[List[Dict[str, Any]]]:
        """
        Query recent air quality data

        Args:
            hours: Number of hours to look back (defaults to config value)

        Returns:
            List of data points or None if error
        """
        if not self.client:
            self.logger.error("InfluxDB client not connected")
            return None

        hours = hours or config.DATA_RETENTION_HOURS

        try:
            end_time = datetime.utcnow()
            start_time = end_time - timedelta(hours=hours)

            query = f'''
                SELECT * FROM "air_quality"
                WHERE time >= '{start_time.isoformat()}Z'
                AND time <= '{end_time.isoformat()}Z'
                AND pm25 > 0
                ORDER BY time DESC
            '''

            result = self.client.query(query)
            points = list(result.get_points())

            self.logger.info(f"Retrieved {len(points)} data points from last {hours} hours")
            return points

        except InfluxDBError as e:
            self.logger.error(f"InfluxDB query error: {e}")
            return None

    def get_device_stats(self, device_id: str, hours: int = 24) -> Optional[Dict[str, Any]]:
        """
        Get statistics for a specific device

        Args:
            device_id: Device identifier
            hours: Time window in hours

        Returns:
            Device statistics or None if error
        """
        if not self.client:
            self.logger.error("InfluxDB client not connected")
            return None

        try:
            end_time = datetime.utcnow()
            start_time = end_time - timedelta(hours=hours)

            query = f'''
                SELECT COUNT(pm25) as count, MEAN(pm25) as avg_pm25,
                       MIN(pm25) as min_pm25, MAX(pm25) as max_pm25
                FROM "air_quality"
                WHERE time >= '{start_time.isoformat()}Z'
                AND device_id = '{device_id}'
            '''

            result = self.client.query(query)
            points = list(result.get_points())

            if points:
                return points[0]
            return None

        except InfluxDBError as e:
            self.logger.error(f"InfluxDB stats query error: {e}")
            return None

    def cleanup_old_data(self, retention_hours: int = None) -> bool:
        """
        Remove data older than retention period

        Args:
            retention_hours: Data retention in hours

        Returns:
            True if successful
        """
        if not self.client:
            self.logger.error("InfluxDB client not connected")
            return False

        retention_hours = retention_hours or config.DATA_RETENTION_HOURS
        cutoff_time = datetime.utcnow() - timedelta(hours=retention_hours)

        try:
            query = f'DELETE FROM "air_quality" WHERE time < \'{cutoff_time.isoformat()}Z\''
            self.client.query(query)
            self.logger.info(f"Cleaned up data older than {retention_hours} hours")
            return True

        except InfluxDBError as e:
            self.logger.error(f"InfluxDB cleanup error: {e}")
            return False