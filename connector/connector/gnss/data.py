"""
User facing data classes for GNSS data.
"""

from enum import Enum
from datetime import datetime
from typing import Optional
from pydantic import BaseModel

class GNSSMode(Enum):
    """GNSS mode enumeration."""
    NO_FIX = 0
    FIX_2D = 1
    FIX_3D = 2

class Position(BaseModel):
    """GNSS position data."""
    time: datetime
    mode: GNSSMode
    latitude: Optional[float] # Latitude in degrees
    longitude: Optional[float] # Longitude in degrees
    speed: Optional[float] # Speed in meters per second
    gps_time: Optional[datetime] # GPS time in UTC
    raw: dict # Raw GNSS data as a dictionary
    last_sky_report: Optional[dict] # Last sky report data as a dictionary
    last_noise_report: Optional[dict] # Last noise report data as a dictionary
