"""
Utility functions.
"""
from .. import CONNECTOR_ROOT_LOGGER

from .config_logging import activate_file_logging
from .serial_connection import SerialConnection

__all__ = ["activate_file_logging", "SerialConnection"]

UTIL_ROOT_LOGGER = CONNECTOR_ROOT_LOGGER.getChild("util")
