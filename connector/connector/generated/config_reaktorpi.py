# Auto-generated from config_reaktorpi.yaml
# DO NOT MODIFY!

# Configuration parameters for the central Rasberry Pi (ReaktorPi) in the control box

SERIAL_PORT_MOTOR = "/dev/serial/by-path/platform-xhci-hcd.0-usb-0:2:1.0-port0" # Serial port for the motor driver.
SERIAL_PORT_POWER = "/dev/serial/by-path/platform-xhci-hcd.1-usb-0:2:1.0-port0" # Serial port for the power meter.
SERIAL_PORT_MOTOR_ENV_KEY = "MOTOR_SERIAL_PORT" # Environment variable key for overriding the motor driver serial port.
SERIAL_PORT_POWER_ENV_KEY = "POWER_SERIAL_PORT" # Environment variable key for overriding the power meter serial port.
