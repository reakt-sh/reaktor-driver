"""
Shared classes package.
"""
from .. import CONNECTOR_ROOT_LOGGER

SHARED_ROOT_LOGGER = CONNECTOR_ROOT_LOGGER.getChild("shared")

# pylint: disable=wrong-import-position
from .serial_connection import SerialConnection
from .data_provider import DataProvider

__all__ = ["SerialConnection", "DataProvider"]
