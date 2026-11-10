// Auto-generated from config_power_meter.yaml
// DO NOT MODIFY!

// Configuration parameters for the power meter

#pragma once
#include <Arduino.h>


// Serial connection settings for the power meter.
const int POWER_METER_SERIAL_BAUDRATE = 9600; // Baud rate for the power meter serial connection.
const int POWER_METER_SERIAL_BYTESIZE = 8; // Byte size for the power meter serial connection.
const char* POWER_METER_SERIAL_PARITY = "N"; // Parity for the power meter serial connection. Value: serial.PARITY_NONE
const int POWER_METER_SERIAL_STOPBITS = 1; // Stop bits for the power meter serial connection. Value: serial.STOPBITS_ONE
const float POWER_METER_SERIAL_TIMEOUT = 1.0; // Timeout in seconds for the power meter serial connection.

// Protocol settings for the power meter.
const int POWER_METER_PROTOCOL_MESSAGE_BYTES = 21; // Number of bytes in a single response from the power meter.
