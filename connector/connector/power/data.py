"""
User facing data classes for power sensor data.
"""

from datetime import datetime
from pydantic import BaseModel

class PowerConsumption(BaseModel):
    """Power consumption data."""
    time: datetime
    voltage: float # Voltage in volts
    current: float # Current in amperes
    power: float   # Power in watts
    energy: float # Energy in watt-hours
    high_voltage_alarm: bool
    low_voltage_alarm: bool
