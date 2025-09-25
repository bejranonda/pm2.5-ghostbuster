#!/usr/bin/env python3
"""
PM2.5 Ghostbuster - Standalone API Server
For running the API server independently

Enhanced for v2.1.0 by Claude Code Assistant
"""

import sys
from pathlib import Path

# Add project root to Python path
project_root = Path(__file__).parent.parent
sys.path.insert(0, str(project_root))

from config.settings import config
from src.utils.logger import get_logger
from src.services.influx_service import InfluxService
from src.services.geojson_service import GeoJSONService
from src.services.alert_service import AlertService
from src.services.api_service import APIService


def main():
    """Main entry point for standalone API server"""
    logger = get_logger('api_server')

    try:
        # Validate configuration
        if not config.validate():
            logger.error("Invalid configuration. Please check your .env file.")
            sys.exit(1)

        # Initialize services
        influx_service = InfluxService()
        geojson_service = GeoJSONService(influx_service)
        alert_service = AlertService()

        # Create API service
        api_service = APIService(influx_service, geojson_service, alert_service)

        # Get configuration
        api_port = getattr(config, 'API_PORT', 5001)
        api_host = getattr(config, 'API_HOST', '0.0.0.0')

        logger.info("Starting PM2.5 Ghostbuster API Server")
        logger.info(f"Version: 2.1.0")
        logger.info(f"Enhanced by Claude Code Assistant")
        logger.info(f"Server: {api_host}:{api_port}")
        logger.info(f"Debug mode: {config.DEBUG}")

        # Run the API server
        api_service.run(host=api_host, port=api_port, debug=config.DEBUG)

    except KeyboardInterrupt:
        logger.info("Shutting down API server...")
    except Exception as e:
        logger.error(f"API server error: {e}")
        sys.exit(1)


if __name__ == "__main__":
    main()