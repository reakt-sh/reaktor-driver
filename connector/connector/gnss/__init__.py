"""
GNSS package.
"""
from .. import CONNECTOR_ROOT_LOGGER

GNSS_ROOT_LOGGER = CONNECTOR_ROOT_LOGGER.getChild("gnss")

from .connection import GNSSConnection
from ..errors import ConnectionProblem

__all__ = ["GNSSConnection", "ConnectionProblem"]
