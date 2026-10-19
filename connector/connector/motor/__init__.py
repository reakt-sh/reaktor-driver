"""
Connector package initialization.
"""
from .. import CONNECTOR_ROOT_LOGGER

MOTOR_ROOT_LOGGER = CONNECTOR_ROOT_LOGGER.getChild("motor")

from .connection import MotorConnection
from .data import Control, Status, Mode, ErrorState, InternalState
from ..errors import ConnectionProblem

__all__ = ["MotorConnection", "Control", "Status", "Mode", "ErrorState", "InternalState", "ConnectionProblem"]

