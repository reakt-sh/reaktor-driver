// Auto-generated from errors.yaml
// DO NOT MODIFY!

// List of error codes and messages used by the driver
// Error codes will encoded in a one-hot bit-vector and should be continuous.

#pragma once
#include <Arduino.h>

const int ERROR_COMM_CONNECTION_VERSION_MISMATCH = 0;
const char* ERROR_COMM_CONNECTION_VERSION_MISMATCH_MESSAGE = "Connection refuesed due to protocol version mismatch between driver and remote control.";
const int ERROR_COMM_CONNECTION_NOT_ESTABLISHED = 1;
const char* ERROR_COMM_CONNECTION_NOT_ESTABLISHED_MESSAGE = "A control message was received without a successful connection handshake beforehand.";
const int ERROR_REMOTE_CONTROL_TIMEOUT = 2;
const char* ERROR_REMOTE_CONTROL_TIMEOUT_MESSAGE = "While in active remote control mode, no control (or heartbeat) message was received in the expected interval.";
const int ERROR_HARDWARE_NO_THROTTLE_DAC = 3;
const char* ERROR_HARDWARE_NO_THROTTLE_DAC_MESSAGE = "No connection to motor throttle control component (DAC via I2C).";
const int ERROR_CONTROL_MODE_SWITCH_UNSAFE = 4;
const char* ERROR_CONTROL_MODE_SWITCH_UNSAFE_MESSAGE = "Control mode switch attempted while in unsafe driving mode.";

// Mapping of error codes to human-readable messages.
const char* ERROR_MAP[] = {
   "Connection refuesed due to protocol version mismatch between driver and remote control.",
   "A control message was received without a successful connection handshake beforehand.",
   "While in active remote control mode, no control (or heartbeat) message was received in the expected interval.",
   "No connection to motor throttle control component (DAC via I2C).",
   "Control mode switch attempted while in unsafe driving mode."
};
