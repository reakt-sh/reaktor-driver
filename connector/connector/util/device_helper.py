"""Internal helper functions for device management."""

import os
from unittest.mock import DEFAULT
from pydantic import BaseModel
from . import UTIL_ROOT_LOGGER

logger = UTIL_ROOT_LOGGER.getChild("devices")

class SerialPorts(BaseModel):
    """Ports of the serial devices."""
    motor: str # Motor driver
    power: str # Power consumption meter

# TODO Move to .env file or config file
DEFAULT_SERIAL_PORTS = SerialPorts(
    motor="/dev/serial/by-path/platform-xhci-hcd.1-usb-0:2:1.0-port0", # USB Port upper right
    power="/dev/serial/by-path/platform-xhci-hcd.0-usb-0:1:1.0-port0", # USB Port upper left
)

async def get_serial_ports() -> SerialPorts:
    """Get the serial ports of the devices."""
    if os.environ.get('USER') == "reaktor":
        # TODO Implement device verification and dynamic port detection
        return DEFAULT_SERIAL_PORTS
    # Just for testing, assumes only Arduino is connected to the USB port
    return SerialPorts(motor="/dev/ttyUSB0", power="/dev/ttyUSB1")
