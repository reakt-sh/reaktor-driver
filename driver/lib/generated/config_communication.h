// Auto-generated from config_communication.yaml
// DO NOT MODIFY!

// Configuration parameters for the communication between driver and connector

#pragma once
#include <Arduino.h>

const uint8_t COMM_PROTOCOL_VERSION = 1; // Version number of the communication protocol.
const uint32_t COMM_SERIAL_BAUDRATE = 115200; // Serial communication speed.
const unsigned int COMM_MESSAGE_ACKNOWLEDGEMENT_CODE_BITSIZE = 8; // Number of bits used to encode the acknowledgement code in status and control messages.
const unsigned int COMM_STATUS_MESSAGE_TIME_BITSIZE = 32; // Number of bits used to encode the internal time in status messages.
const unsigned int COMM_STATUS_MESSAGE_SPACING_TIME = 10; // Minimal time between two status messages in ms (unless enforced response).
const unsigned int COMM_CONTROL_MESSAGE_HEARTBEAT_TIME = 1000; // When in remote control mode, the maximum time in ms between two control messages before the driver stops the motor for safety reasons.
const unsigned int COMM_CONTROL_MESSAGE_ACKNOWLEDGEMENT_TIME = 1000; // Maximum time in ms between a sent control message and its acknowledgement by a status message (w.r.t. acknowledgement code).
