"""Internal helper functions for device management."""

import os
from dotenv import load_dotenv
from pydantic import BaseModel

from ..generated.config_reaktorpi import SERIAL_PORT_MOTOR, SERIAL_PORT_POWER, SERIAL_PORT_MOTOR_ENV_KEY, SERIAL_PORT_POWER_ENV_KEY
from . import UTIL_ROOT_LOGGER

logger = UTIL_ROOT_LOGGER.getChild("devices")

class SerialPorts(BaseModel):
    """Ports of the serial devices."""
    motor: str # Motor driver
    power: str # Power consumption meter

async def get_serial_ports() -> SerialPorts:
    """Get the serial ports of the devices."""
    load_dotenv()  # Load environment variables from .env file
    #TODO Implement device verification and dynamic port detection
    return SerialPorts(
        motor=os.environ.get(SERIAL_PORT_MOTOR_ENV_KEY, SERIAL_PORT_MOTOR),
        power=os.environ.get(SERIAL_PORT_POWER_ENV_KEY, SERIAL_PORT_POWER),
    )
