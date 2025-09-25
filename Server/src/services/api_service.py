"""
PM2.5 Ghostbuster - REST API Service
Provides HTTP API endpoints for data access and system monitoring

Enhanced for v2.1.0 by Claude Code Assistant
"""

from datetime import datetime, timedelta
from typing import Dict, List, Optional
from flask import Flask, jsonify, request, Response
from flask_cors import CORS
import json

from config.settings import config
from src.utils.logger import get_logger
from src.services.influx_service import InfluxService
from src.services.geojson_service import GeoJSONService
from src.services.alert_service import AlertService
from src.utils.timezone_utils import tz_manager


class APIService:
    """REST API service for PM2.5 Ghostbuster"""

    def __init__(self, influx_service: InfluxService = None,
                 geojson_service: GeoJSONService = None,
                 alert_service: AlertService = None):
        """Initialize API service"""
        self.logger = get_logger('api_service')
        self.app = Flask(__name__)
        CORS(self.app)  # Enable CORS for web frontend

        # Services
        self.influx_service = influx_service or InfluxService()
        self.geojson_service = geojson_service or GeoJSONService(self.influx_service)
        self.alert_service = alert_service or AlertService()

        self._setup_routes()

    def _setup_routes(self):
        """Setup API routes"""

        @self.app.route('/api/v1/health', methods=['GET'])
        def health_check():
            """Health check endpoint"""
            try:
                health_status = {
                    'status': 'healthy',
                    'timestamp': tz_manager.now_local().isoformat(),
                    'services': {
                        'influxdb': self.influx_service.is_connected(),
                        'geojson': True,  # Always available
                        'alerts': True    # Always available
                    },
                    'version': '2.1.0'
                }

                # Overall health
                health_status['healthy'] = all(health_status['services'].values())

                status_code = 200 if health_status['healthy'] else 503
                return jsonify(health_status), status_code

            except Exception as e:
                self.logger.error(f"Health check error: {e}")
                return jsonify({'status': 'error', 'message': str(e)}), 500

        @self.app.route('/api/v1/data/current', methods=['GET'])
        def get_current_data():
            """Get current GeoJSON data"""
            try:
                hours = request.args.get('hours', 24, type=int)
                hours = min(max(hours, 1), 168)  # Limit between 1 and 168 hours

                geojson_data = self.geojson_service.generate_geojson(hours)
                if geojson_data is None:
                    return jsonify({'error': 'Failed to generate data'}), 500

                return Response(
                    json.dumps(geojson_data),
                    mimetype='application/geo+json',
                    headers={'Cache-Control': 'max-age=60'}  # Cache for 1 minute
                )

            except Exception as e:
                self.logger.error(f"Current data error: {e}")
                return jsonify({'error': str(e)}), 500

        @self.app.route('/api/v1/data/summary', methods=['GET'])
        def get_data_summary():
            """Get data summary statistics"""
            try:
                summary = self.geojson_service.get_summary_stats()
                if summary is None:
                    return jsonify({'error': 'No data available'}), 404

                return jsonify(summary)

            except Exception as e:
                self.logger.error(f"Summary data error: {e}")
                return jsonify({'error': str(e)}), 500

        @self.app.route('/api/v1/devices', methods=['GET'])
        def get_devices():
            """Get list of active devices"""
            try:
                data_points = self.influx_service.query_recent_data(24)
                if not data_points:
                    return jsonify([])

                devices = {}
                for point in data_points:
                    device_id = point.get('device_id')
                    if device_id and device_id not in devices:
                        devices[device_id] = {
                            'device_id': device_id,
                            'last_seen': point.get('time'),
                            'location': {
                                'latitude': point.get('latitude'),
                                'longitude': point.get('longitude')
                            }
                        }
                    elif device_id:
                        # Update with more recent data if available
                        if point.get('time') > devices[device_id]['last_seen']:
                            devices[device_id]['last_seen'] = point.get('time')
                            devices[device_id]['location'] = {
                                'latitude': point.get('latitude'),
                                'longitude': point.get('longitude')
                            }

                return jsonify(list(devices.values()))

            except Exception as e:
                self.logger.error(f"Devices endpoint error: {e}")
                return jsonify({'error': str(e)}), 500

        @self.app.route('/api/v1/devices/<device_id>/stats', methods=['GET'])
        def get_device_stats(device_id: str):
            """Get statistics for specific device"""
            try:
                hours = request.args.get('hours', 24, type=int)
                hours = min(max(hours, 1), 168)

                stats = self.influx_service.get_device_stats(device_id, hours)
                if stats is None:
                    return jsonify({'error': 'Device not found or no data'}), 404

                return jsonify(stats)

            except Exception as e:
                self.logger.error(f"Device stats error: {e}")
                return jsonify({'error': str(e)}), 500

        @self.app.route('/api/v1/alerts', methods=['GET'])
        def get_alerts():
            """Get active alerts"""
            try:
                active_alerts = self.alert_service.get_active_alerts()
                alerts_data = []

                for alert in active_alerts:
                    alerts_data.append({
                        'device_id': alert.device_id,
                        'level': alert.level.value,
                        'pm25_value': alert.pm25_value,
                        'location': alert.location,
                        'timestamp': alert.timestamp.isoformat(),
                        'message': alert.message,
                        'acknowledged': alert.acknowledged
                    })

                return jsonify(alerts_data)

            except Exception as e:
                self.logger.error(f"Alerts endpoint error: {e}")
                return jsonify({'error': str(e)}), 500

        @self.app.route('/api/v1/alerts/summary', methods=['GET'])
        def get_alert_summary():
            """Get alert summary statistics"""
            try:
                summary = self.alert_service.get_alert_summary()
                return jsonify(summary)

            except Exception as e:
                self.logger.error(f"Alert summary error: {e}")
                return jsonify({'error': str(e)}), 500

        @self.app.route('/api/v1/alerts/<device_id>/acknowledge', methods=['POST'])
        def acknowledge_alert(device_id: str):
            """Acknowledge an alert for a device"""
            try:
                success = self.alert_service.acknowledge_alert(device_id)
                if success:
                    return jsonify({'message': 'Alert acknowledged'})
                else:
                    return jsonify({'error': 'No active alert for device'}), 404

            except Exception as e:
                self.logger.error(f"Alert acknowledgment error: {e}")
                return jsonify({'error': str(e)}), 500

        @self.app.route('/api/v1/system/info', methods=['GET'])
        def get_system_info():
            """Get system information"""
            try:
                return jsonify({
                    'version': '2.1.0',
                    'name': 'PM2.5 Ghostbuster',
                    'description': 'Making invisible air pollution visible',
                    'philosophy': 'PM2.5 is like ghost. It exists even if it is invisible.',
                    'timezone': config.DEFAULT_TIMEZONE,
                    'data_retention_hours': config.DATA_RETENTION_HOURS,
                    'update_interval': config.GEOJSON_UPDATE_INTERVAL,
                    'features': [
                        'Real-time PM2.5 monitoring',
                        'GPS-tracked mobile sensors',
                        'Automated alerts and notifications',
                        'Historical data analysis',
                        'REST API access',
                        'Web-based visualization'
                    ],
                    'enhanced_by': 'Claude Code Assistant'
                })

            except Exception as e:
                self.logger.error(f"System info error: {e}")
                return jsonify({'error': str(e)}), 500

        @self.app.route('/api/v1/data/export', methods=['GET'])
        def export_data():
            """Export data for specified time period"""
            try:
                # Get parameters
                start_str = request.args.get('start')
                end_str = request.args.get('end')
                format_type = request.args.get('format', 'json').lower()

                if not start_str or not end_str:
                    return jsonify({'error': 'start and end parameters required'}), 400

                # Parse dates
                try:
                    start_time = datetime.fromisoformat(start_str.replace('Z', '+00:00'))
                    end_time = datetime.fromisoformat(end_str.replace('Z', '+00:00'))
                except ValueError:
                    return jsonify({'error': 'Invalid date format. Use ISO format.'}), 400

                # Limit export period (max 30 days)
                max_period = timedelta(days=30)
                if end_time - start_time > max_period:
                    return jsonify({'error': 'Export period cannot exceed 30 days'}), 400

                # Get data
                query = f'''
                    SELECT * FROM "air_quality"
                    WHERE time >= '{start_time.isoformat()}Z'
                    AND time <= '{end_time.isoformat()}Z'
                    ORDER BY time DESC
                '''

                result = self.influx_service.client.query(query)
                data_points = list(result.get_points())

                if format_type == 'csv':
                    # Return CSV format
                    import csv
                    from io import StringIO

                    output = StringIO()
                    if data_points:
                        writer = csv.DictWriter(output, fieldnames=data_points[0].keys())
                        writer.writeheader()
                        writer.writerows(data_points)

                    return Response(
                        output.getvalue(),
                        mimetype='text/csv',
                        headers={'Content-Disposition': f'attachment; filename=pm25_data_{start_str}_{end_str}.csv'}
                    )
                else:
                    # Return JSON format
                    return jsonify({
                        'start_time': start_str,
                        'end_time': end_str,
                        'total_records': len(data_points),
                        'data': data_points
                    })

            except Exception as e:
                self.logger.error(f"Data export error: {e}")
                return jsonify({'error': str(e)}), 500

        @self.app.errorhandler(404)
        def not_found(error):
            return jsonify({'error': 'Endpoint not found'}), 404

        @self.app.errorhandler(500)
        def internal_error(error):
            return jsonify({'error': 'Internal server error'}), 500

    def run(self, host: str = '0.0.0.0', port: int = 5000, debug: bool = False):
        """
        Run the API server

        Args:
            host: Host to bind to
            port: Port to bind to
            debug: Enable debug mode
        """
        self.logger.info(f"Starting PM2.5 Ghostbuster API server on {host}:{port}")
        self.app.run(host=host, port=port, debug=debug)

    def get_app(self) -> Flask:
        """Get Flask application instance"""
        return self.app