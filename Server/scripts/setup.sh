#!/bin/bash
# PM2.5 Ghostbuster - Setup Script
# Automated setup for PM2.5 Ghostbuster v2.1.0
# Enhanced by Claude Code Assistant

set -e  # Exit on any error

echo "🌟 PM2.5 Ghostbuster v2.1.0 Setup"
echo "Enhanced by Claude Code Assistant"
echo "=================================="

# Colors for output
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
BLUE='\033[0;34m'
NC='\033[0m' # No Color

# Function to print colored output
print_status() {
    echo -e "${GREEN}[INFO]${NC} $1"
}

print_warning() {
    echo -e "${YELLOW}[WARN]${NC} $1"
}

print_error() {
    echo -e "${RED}[ERROR]${NC} $1"
}

print_step() {
    echo -e "${BLUE}[STEP]${NC} $1"
}

# Check if running as root
if [[ $EUID -eq 0 ]]; then
    print_error "This script should not be run as root for security reasons"
    exit 1
fi

# Check OS compatibility
if [[ "$OSTYPE" != "linux-gnu"* ]] && [[ "$OSTYPE" != "darwin"* ]]; then
    print_error "This script only supports Linux and macOS"
    exit 1
fi

print_step "Checking system requirements..."

# Check Python version
if ! command -v python3 &> /dev/null; then
    print_error "Python 3 is required but not installed"
    exit 1
fi

PYTHON_VERSION=$(python3 -c 'import sys; print(".".join(map(str, sys.version_info[:2])))')
if ! python3 -c 'import sys; exit(0 if sys.version_info >= (3, 8) else 1)'; then
    print_error "Python 3.8 or higher is required (found $PYTHON_VERSION)"
    exit 1
fi

print_status "Python $PYTHON_VERSION found"

# Check pip
if ! command -v pip3 &> /dev/null; then
    print_error "pip3 is required but not installed"
    exit 1
fi

print_step "Setting up Python virtual environment..."

# Create virtual environment
if [ ! -d "venv" ]; then
    python3 -m venv venv
    print_status "Virtual environment created"
else
    print_status "Virtual environment already exists"
fi

# Activate virtual environment
source venv/bin/activate
print_status "Virtual environment activated"

print_step "Installing Python dependencies..."

# Upgrade pip
pip install --upgrade pip

# Install requirements
pip install -r requirements.txt
print_status "Python dependencies installed"

print_step "Setting up configuration..."

# Create .env file if it doesn't exist
if [ ! -f "config/.env" ]; then
    cp config/.env.example config/.env
    print_warning "Configuration file created from template"
    print_warning "Please edit config/.env with your settings before running"
else
    print_status "Configuration file already exists"
fi

# Create log directory
LOG_DIR="/var/log/pm25"
if [ ! -d "$LOG_DIR" ]; then
    if sudo mkdir -p "$LOG_DIR" 2>/dev/null && sudo chown $USER:$USER "$LOG_DIR" 2>/dev/null; then
        print_status "Log directory created: $LOG_DIR"
    else
        print_warning "Could not create system log directory, using local logs"
        mkdir -p logs
        # Update .env to use local logs
        if grep -q "LOG_PATH=" config/.env; then
            sed -i.bak "s|LOG_PATH=.*|LOG_PATH=./logs/|" config/.env
        else
            echo "LOG_PATH=./logs/" >> config/.env
        fi
    fi
else
    print_status "Log directory exists: $LOG_DIR"
fi

# Create data directories
mkdir -p data/geojson
mkdir -p data/csv
print_status "Data directories created"

print_step "Checking optional dependencies..."

# Check InfluxDB
if command -v influx &> /dev/null; then
    print_status "InfluxDB CLI found"
else
    print_warning "InfluxDB CLI not found - you may need to install it manually"
fi

# Check PM2
if command -v pm2 &> /dev/null; then
    print_status "PM2 process manager found"
else
    print_warning "PM2 not found - install with: npm install -g pm2"
fi

print_step "Running system health check..."

# Test configuration loading
if python3 -c "
import sys
sys.path.insert(0, '.')
try:
    from config.settings import config
    print('✅ Configuration loaded successfully')
except Exception as e:
    print(f'❌ Configuration error: {e}')
    sys.exit(1)
"; then
    print_status "Configuration test passed"
else
    print_error "Configuration test failed"
    exit 1
fi

# Create systemd service file (optional)
if command -v systemctl &> /dev/null; then
    cat > pm25-ghostbuster.service << EOF
[Unit]
Description=PM2.5 Ghostbuster Data Collector
After=network.target

[Service]
Type=simple
User=$USER
WorkingDirectory=$(pwd)
Environment=PATH=$(pwd)/venv/bin
ExecStart=$(pwd)/venv/bin/python src/main_data_collector.py
Restart=always
RestartSec=10

[Install]
WantedBy=multi-user.target
EOF
    print_status "Systemd service file created: pm25-ghostbuster.service"
    print_warning "To install: sudo cp pm25-ghostbuster.service /etc/systemd/system/"
fi

print_step "Setup completed successfully! 🎉"
echo
echo "Next steps:"
echo "1. Edit config/.env with your MQTT and database settings"
echo "2. Test the configuration: python3 src/main_data_collector.py"
echo "3. For production, use PM2: pm2 start ecosystem.config.js"
echo
echo "Documentation:"
echo "- README.md - Project overview"
echo "- DEPLOYMENT.md - Detailed setup guide"
echo "- API docs: http://localhost:5000/api/v1/system/info (when running)"
echo
echo "🌟 PM2.5 Ghostbuster is ready!"
echo "\"PM2.5 is like ghost. It exists even if it is invisible.\""
echo
echo "Enhanced by Claude Code Assistant 🤖"