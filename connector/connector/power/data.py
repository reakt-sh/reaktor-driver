"""
User facing data classes for power sensor data.
"""

from datetime import datetime
from pydantic import BaseModel

class PowerConsumption(BaseModel):
    """Power consumption data."""
    received_at: datetime
    voltage_v: float # Voltage in volts
    current_a: float # Current in amperes
    power_w: float   # Power in watts
    energy_wh: float # Energy in watt-hours
    high_voltage_alarm: bool
    low_voltage_alarm: bool
