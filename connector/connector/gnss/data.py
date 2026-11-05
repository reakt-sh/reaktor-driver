"""
User facing data classes for GNSS data.
"""

from datetime import datetime
from pydantic import BaseModel

class Position(BaseModel):
    """GNSS position data."""
    time: datetime
    latitude: float # Latitude in degrees
    longitude: float # Longitude in degrees
    altitude: float   # Altitude in meters
    speed: float # Speed in km/h
