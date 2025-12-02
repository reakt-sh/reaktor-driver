// Auto-generated from config_communication.yaml
// DO NOT MODIFY!

// Configuration parameters for the communication between driver and connector

#pragma once
#include <Arduino.h>

const uint32_t COMM_SERIAL_BAUDRATE = 115200; // Serial communication speed.
const unsigned int COMM_STATUS_MESSAGE_TIME_BITSIZE = 32; // Number of bits used to encode the internal time in status messages.
const unsigned int COMM_STATUS_MESSAGE_SPACING_TIME = 10; // Minimal time between two status messages in ms (unless enforced response).
const unsigned int COMM_CONTROL_MESSAGE_HEARTBEAT_TIME = 1000; // When in remote control mode, the maximum time in ms between two control messages before the driver stops the motor for safety reasons.
