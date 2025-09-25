#!/usr/bin/env python3
"""
PM2.5 Ghostbuster - Health Check Script
Comprehensive system health monitoring

Enhanced for v2.1.0 by Claude Code Assistant
"""

import sys
import time
import json
import requests
from datetime import datetime, timedelta
from pathlib import Path

# Add project root to Python path
project_root = Path(__file__).parent.parent
sys.path.insert(0, str(project_root))

from config.settings import config
from src.utils.logger import get_logger
from src.services.influx_service import InfluxService
from src.utils.timezone_utils import tz_manager


class HealthChecker:
    """Comprehensive health checking for PM2.5 Ghostbuster"""

    def __init__(self):
        self.logger = get_logger('health_check')
        self.influx_service = InfluxService()
        self.results = {}

    def check_influxdb(self) -> bool:
        """Check InfluxDB connectivity and data freshness"""
        try:
            # Check connection
            if not self.influx_service.is_connected():
                self.results['influxdb'] = {
                    'status': 'error',
                    'message': 'Cannot connect to InfluxDB'
                }
                return False

            # Check recent data
            recent_data = self.influx_service.query_recent_data(1)  # Last hour
            if not recent_data:
                self.results['influxdb'] = {
                    'status': 'warning',
                    'message': 'No data in last hour',
                    'connected': True
                }
                return True

            # Check data freshness
            latest_time = max([point.get('time') for point in recent_data])
            latest_dt = datetime.fromisoformat(latest_time.replace('Z', '+00:00'))
            time_diff = datetime.utcnow() - latest_dt.replace(tzinfo=None)

            if time_diff > timedelta(minutes=30):
                status = 'warning'
                message = f'Latest data is {time_diff} old'
            else:
                status = 'healthy'
                message = f'Latest data: {time_diff} ago'

            self.results['influxdb'] = {
                'status': status,
                'message': message,
                'connected': True,
                'latest_data': latest_time,
                'data_points_last_hour': len(recent_data)
            }
            return True

        except Exception as e:
            self.results['influxdb'] = {
                'status': 'error',
                'message': str(e)
            }
            return False

    def check_api_server(self) -> bool:
        """Check API server health"""
        try:
            api_port = getattr(config, 'API_PORT', 5000)
            url = f'http://localhost:{api_port}/api/v1/health'

            response = requests.get(url, timeout=10)
            response.raise_for_status()

            health_data = response.json()

            self.results['api_server'] = {
                'status': 'healthy' if health_data.get('healthy') else 'warning',
                'message': 'API server responding',
                'url': url,
                'response_time': response.elapsed.total_seconds(),
                'details': health_data
            }
            return health_data.get('healthy', False)

        except requests.exceptions.ConnectionError:
            self.results['api_server'] = {
                'status': 'error',
                'message': 'API server not responding'
            }
            return False
        except Exception as e:
            self.results['api_server'] = {
                'status': 'error',
                'message': str(e)
            }
            return False

    def check_mqtt_connectivity(self) -> bool:
        """Check MQTT broker connectivity"""
        try:
            import paho.mqtt.client as mqtt

            def on_connect(client, userdata, flags, rc):
                userdata['connected'] = (rc == 0)
                userdata['rc'] = rc

            def on_disconnect(client, userdata, rc):
                userdata['connected'] = False

            userdata = {'connected': False, 'rc': None}
            client = mqtt.Client(userdata=userdata)
            client.on_connect = on_connect
            client.on_disconnect = on_disconnect

            if config.MQTT_USERNAME and config.MQTT_PASSWORD:
                client.username_pw_set(config.MQTT_USERNAME, config.MQTT_PASSWORD)

            client.connect(config.MQTT_BROKER_ADDRESS, config.MQTT_BROKER_PORT, 10)
            client.loop_start()

            # Wait for connection
            timeout = 10
            start_time = time.time()
            while not userdata['connected'] and time.time() - start_time < timeout:
                time.sleep(0.1)

            client.loop_stop()
            client.disconnect()

            if userdata['connected']:
                self.results['mqtt'] = {
                    'status': 'healthy',
                    'message': 'MQTT broker connected successfully',
                    'broker': f"{config.MQTT_BROKER_ADDRESS}:{config.MQTT_BROKER_PORT}"
                }
                return True
            else:
                self.results['mqtt'] = {
                    'status': 'error',
                    'message': f'MQTT connection failed (code: {userdata["rc"]})',
                    'broker': f"{config.MQTT_BROKER_ADDRESS}:{config.MQTT_BROKER_PORT}"
                }
                return False

        except Exception as e:
            self.results['mqtt'] = {
                'status': 'error',
                'message': str(e)
            }
            return False

    def check_disk_space(self) -> bool:
        """Check disk space for critical paths"""
        try:
            import shutil

            paths_to_check = [
                config.LOG_PATH if hasattr(config, 'LOG_PATH') and config.LOG_PATH else './logs',
                './data',
                '.'  # Current directory
            ]

            disk_info = {}
            all_ok = True

            for path in paths_to_check:
                try:
                    if not Path(path).exists():
                        continue

                    total, used, free = shutil.disk_usage(path)
                    free_percent = (free / total) * 100

                    status = 'healthy'
                    if free_percent < 5:
                        status = 'error'
                        all_ok = False
                    elif free_percent < 15:
                        status = 'warning'

                    disk_info[path] = {
                        'total_gb': round(total / (1024**3), 1),
                        'free_gb': round(free / (1024**3), 1),
                        'free_percent': round(free_percent, 1),
                        'status': status
                    }

                except Exception as e:
                    disk_info[path] = {'error': str(e)}

            self.results['disk_space'] = {
                'status': 'healthy' if all_ok else 'warning',
                'message': 'Disk space check completed',
                'paths': disk_info
            }
            return all_ok

        except Exception as e:
            self.results['disk_space'] = {
                'status': 'error',
                'message': str(e)
            }
            return False

    def check_system_resources(self) -> bool:
        """Check system resources (CPU, memory)"""
        try:
            import psutil

            # Memory check
            memory = psutil.virtual_memory()
            memory_percent = memory.percent

            # CPU check
            cpu_percent = psutil.cpu_percent(interval=1)

            # Load average (Linux/macOS)
            try:
                load_avg = psutil.getloadavg()
            except (AttributeError, OSError):
                load_avg = None

            status = 'healthy'
            issues = []

            if memory_percent > 90:
                status = 'error'
                issues.append('High memory usage')
            elif memory_percent > 80:
                status = 'warning'
                issues.append('Elevated memory usage')

            if cpu_percent > 80:
                if status != 'error':
                    status = 'warning'
                issues.append('High CPU usage')

            self.results['system_resources'] = {
                'status': status,
                'message': '; '.join(issues) if issues else 'System resources OK',
                'memory_percent': memory_percent,
                'cpu_percent': cpu_percent,
                'load_average': load_avg
            }

            return status != 'error'

        except ImportError:
            self.results['system_resources'] = {
                'status': 'warning',
                'message': 'psutil not available for resource monitoring'
            }
            return True
        except Exception as e:
            self.results['system_resources'] = {
                'status': 'error',
                'message': str(e)
            }
            return False

    def run_all_checks(self) -> dict:
        """Run all health checks"""
        self.logger.info("Starting comprehensive health check...")

        checks = {
            'InfluxDB': self.check_influxdb,
            'API Server': self.check_api_server,
            'MQTT Broker': self.check_mqtt_connectivity,
            'Disk Space': self.check_disk_space,
            'System Resources': self.check_system_resources
        }

        overall_status = True
        for check_name, check_func in checks.items():
            self.logger.info(f"Checking {check_name}...")
            try:
                result = check_func()
                if not result:
                    overall_status = False
            except Exception as e:
                self.logger.error(f"Error in {check_name} check: {e}")
                overall_status = False

        # Overall summary
        summary = {
            'overall_status': 'healthy' if overall_status else 'unhealthy',
            'timestamp': tz_manager.now_local().isoformat(),
            'checks_passed': sum(1 for r in self.results.values()
                                if r.get('status') == 'healthy'),
            'checks_total': len(self.results),
            'details': self.results
        }

        self.logger.info(f"Health check completed: {summary['overall_status']}")
        return summary


def main():
    """Main entry point"""
    checker = HealthChecker()
    results = checker.run_all_checks()

    # Print results
    print(json.dumps(results, indent=2))

    # Exit with appropriate code
    if results['overall_status'] == 'healthy':
        sys.exit(0)
    else:
        sys.exit(1)


if __name__ == "__main__":
    main()