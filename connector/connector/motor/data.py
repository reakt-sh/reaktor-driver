"""
User facing data classes for vehicle control and status.
"""

from datetime import datetime
from enum import IntEnum, unique
from typing import Annotated, Optional, List
from pydantic import BaseModel, Field
from ..generated.communication_bp import Mode as MessageMode, DetectedMovement as MessageDetectedMovement, ErrorState

@unique
class Mode(IntEnum):
    """Driving modes of the vehicle."""
    NEUTRAL = MessageMode.DRIVE_MODE_NEUTRAL
    FORWARD = MessageMode.DRIVE_MODE_FORWARD
    REVERSE = MessageMode.DRIVE_MODE_REVERSE
    PARKING = MessageMode.DRIVE_MODE_PARKING
    EMERGENCY_STOP = MessageMode.DRIVE_MODE_EMERGENCY_STOP

@unique
class DetectedMovement(IntEnum):
    """Movement state of the vehicle inferred by the control system."""
    NONE = MessageDetectedMovement.NO_MOVEMENT
    FORWARD = MessageDetectedMovement.MOVEMENT_FORWARD
    REVERSE = MessageDetectedMovement.MOVEMENT_REVERSE
    UNKNOWN_DIRECTION = MessageDetectedMovement.MOVEMENT_UNKNOWN_DIRECTION

type SpeedType = Annotated[float, Field(ge=0, allow_inf_nan=False, description="Speed in m/s")]

class Control(BaseModel):
    """Control command for the vehicle."""
    mode: Mode
    target_speed: SpeedType = 0 # in m/s

class InternalState(BaseModel):
    """Internal state of the driver."""
    time_ms: int
    target_rpm: int
    motor_rpm: int
    detected_movement: DetectedMovement
    switch_remote_control: bool
    switch_forward: bool
    switch_reverse: bool
    throttle_lever_rpm: int
    control_rpm: int
    brake_value: bool

class Status(BaseModel):
    """Status of the vehicle."""
    # Message state
    received_at: datetime
    # Errors
    error: ErrorState
    errors: List[str] = []
    # System state
    remote_control: bool
    connection_established: bool
    # Motor control state
    mode: Mode
    target_speed: float # in m/s
    motor_speed: float # in m/s
    # Internal state
    internal_state: Optional[InternalState] = None
