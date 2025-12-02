from datetime import datetime
from enum import IntEnum, unique
from typing import Optional, List
from pydantic import BaseModel
from .generated.communication_bp import Mode as MessageMode, ErrorState

@unique
class Mode(IntEnum):
    """Driving modes of the vehicle."""
    NEUTRAL = MessageMode.DRIVE_MODE_NEUTRAL
    FORWARD = MessageMode.DRIVE_MODE_FORWARD
    REVERSE = MessageMode.DRIVE_MODE_REVERSE
    PARKING = MessageMode.DRIVE_MODE_PARKING
    EMERGENCY_STOP = MessageMode.DRIVE_MODE_EMERGENCY_STOP

class Control(BaseModel):
    """Control command for the vehicle."""
    mode: Mode
    target_rpm: int = 0

class InternalState(BaseModel):
    """Internal state of the driver."""
    time_ms: int
    control_rpm: int

class Status(BaseModel):
    """Status of the vehicle."""
    # Message state
    received_at: datetime
    # System state
    remote_control: bool
    error: ErrorState
    errors: List[str] = []
    # Motor control state
    mode: Mode
    target_rpm: int
    motor_rpm: int
    # Internal state
    internal_state: Optional[InternalState] = None
