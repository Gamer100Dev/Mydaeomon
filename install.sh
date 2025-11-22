#!/bin/bash

# Setup LED Module systemd service
# Run this script with sudo

SERVICE_NAME="led_module.service"
SERVICE_PATH="/etc/systemd/system/$SERVICE_NAME"
MODULE_DIR="/home/gamer/LED_MODULE"
EXECUTABLE="LED_MODULE"  # Name of your executable

# Check if running as root
if [ "$EUID" -ne 0 ]; then
    echo "Please run this script with sudo."
    exit 1
fi

# Check if executable exists
if [ ! -f "$MODULE_DIR/$EXECUTABLE" ]; then
    echo "Error: executable not found at $MODULE_DIR/$EXECUTABLE"
    exit 1
fi

# Create the systemd service file
echo "Creating systemd service at $SERVICE_PATH..."
cat <<EOL > "$SERVICE_PATH"
[Unit]
Description=LED Module Service
After=network.target

[Service]
Type=simple
User=root
WorkingDirectory=$MODULE_DIR
ExecStart=$MODULE_DIR/$EXECUTABLE
Restart=on-failure
StandardOutput=journal
StandardError=journal

[Install]
WantedBy=multi-user.target
EOL

# Reload systemd
echo "Reloading systemd..."
systemctl daemon-reload

# Enable service at boot
echo "Enabling LED module service..."
systemctl enable "$SERVICE_NAME"

# Start service immediately
echo "Starting LED module service..."
systemctl start "$SERVICE_NAME"

# Show status
echo "Service status:"
systemctl status "$SERVICE_NAME" --no-pager
