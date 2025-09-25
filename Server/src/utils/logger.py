"""
Logging utilities for PM2.5 Ghostbuster
Provides structured logging with timezone awareness
"""

import logging
import sys
from pathlib import Path
from datetime import datetime
from typing import Optional
import pytz

from config.settings import config


class TimezoneFormatter(logging.Formatter):
    """Custom formatter that displays time in Bangkok timezone"""

    def __init__(self, fmt: Optional[str] = None, datefmt: Optional[str] = None):
        super().__init__(fmt, datefmt)
        self.timezone = pytz.timezone(config.DEFAULT_TIMEZONE)

    def formatTime(self, record, datefmt=None):
        """Format timestamp in Bangkok timezone"""
        dt = datetime.fromtimestamp(record.created, tz=self.timezone)
        if datefmt:
            return dt.strftime(datefmt)
        return dt.strftime('%Y-%m-%d %H:%M:%S %Z')


def setup_logger(name: str, log_file: Optional[str] = None) -> logging.Logger:
    """
    Set up logger with file and console handlers

    Args:
        name: Logger name
        log_file: Optional log file path

    Returns:
        Configured logger instance
    """
    logger = logging.getLogger(name)

    # Avoid duplicate handlers
    if logger.handlers:
        return logger

    logger.setLevel(getattr(logging, config.LOG_LEVEL))

    # Create formatters
    formatter = TimezoneFormatter(
        fmt='%(asctime)s - %(name)s - %(levelname)s - %(message)s',
        datefmt='%Y-%m-%d %H:%M:%S %Z'
    )

    # Console handler
    console_handler = logging.StreamHandler(sys.stdout)
    console_handler.setLevel(logging.INFO)
    console_handler.setFormatter(formatter)
    logger.addHandler(console_handler)

    # File handler (if specified)
    if log_file:
        # Ensure log directory exists
        log_path = Path(log_file)
        log_path.parent.mkdir(parents=True, exist_ok=True)

        file_handler = logging.FileHandler(log_file)
        file_handler.setLevel(logging.DEBUG)
        file_handler.setFormatter(formatter)
        logger.addHandler(file_handler)

    return logger


def get_logger(name: str) -> logging.Logger:
    """Get logger for a module"""
    log_file = Path(config.LOG_PATH) / f"{name}.log" if config.LOG_PATH else None
    return setup_logger(name, str(log_file) if log_file else None)