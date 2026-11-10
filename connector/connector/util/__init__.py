"""
Utility functions.
"""
from .. import CONNECTOR_ROOT_LOGGER

UTIL_ROOT_LOGGER = CONNECTOR_ROOT_LOGGER.getChild("util")

# pylint: disable=wrong-import-position
from .config_logging import activate_file_logging

__all__ = ["activate_file_logging"]
