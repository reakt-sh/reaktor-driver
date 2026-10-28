'''Configuration for the connection to the power meter.'''

import serial

# Serial connection settings
POWER_METER_BAUDRATE=9600
POWER_METER_BYTESIZE=8
POWER_METER_PARITY=serial.PARITY_NONE
POWER_METER_STOPBITS=serial.STOPBITS_ONE
POWER_METER_TIMEOUT=1.0

# Protocol settings
MESSAGE_BYTES = 21
