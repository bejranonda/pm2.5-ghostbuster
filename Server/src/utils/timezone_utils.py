"""
Timezone utilities for PM2.5 Ghostbuster
Handles timezone conversions between UTC and Bangkok time
"""

from datetime import datetime
from typing import Union
import pytz

from config.settings import config


class TimezoneManager:
    """Manages timezone conversions for the application"""

    def __init__(self):
        self.utc_tz = pytz.UTC
        self.local_tz = pytz.timezone(config.DEFAULT_TIMEZONE)

    def utc_to_local(self, utc_time: Union[datetime, str]) -> datetime:
        """
        Convert UTC time to local timezone

        Args:
            utc_time: UTC datetime object or ISO string

        Returns:
            Local timezone datetime
        """
        if isinstance(utc_time, str):
            utc_time = datetime.fromisoformat(utc_time.replace('Z', '+00:00'))

        if utc_time.tzinfo is None:
            utc_time = self.utc_tz.localize(utc_time)

        return utc_time.astimezone(self.local_tz)

    def local_to_utc(self, local_time: datetime) -> datetime:
        """
        Convert local time to UTC

        Args:
            local_time: Local timezone datetime

        Returns:
            UTC datetime
        """
        if local_time.tzinfo is None:
            local_time = self.local_tz.localize(local_time)

        return local_time.astimezone(self.utc_tz)

    def now_local(self) -> datetime:
        """Get current time in local timezone"""
        return datetime.now(self.local_tz)

    def now_utc(self) -> datetime:
        """Get current time in UTC"""
        return datetime.now(self.utc_tz)

    def format_local_time(self, dt: datetime, format_str: str = '%Y-%m-%d %H:%M:%S') -> str:
        """
        Format datetime in local timezone

        Args:
            dt: Datetime to format
            format_str: Format string

        Returns:
            Formatted time string
        """
        if dt.tzinfo is None:
            dt = self.utc_tz.localize(dt)

        local_dt = dt.astimezone(self.local_tz)
        return local_dt.strftime(format_str)


# Global timezone manager instance
tz_manager = TimezoneManager()