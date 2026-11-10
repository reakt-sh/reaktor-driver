// Auto-generated from errors.yaml
// DO NOT MODIFY!

// List of error codes and messages used by the driver
// Error codes will encoded in a one-hot bit-vector and should be continuous.

#pragma once
#include <Arduino.h>

const int ERROR_COMM_CONNECTION_VERSION_MISMATCH = 0;
const char* ERROR_COMM_CONNECTION_VERSION_MISMATCH_MESSAGE = "Connection refused due to protocol version mismatch between driver and remote control.";
const int ERROR_COMM_CONNECTION_NOT_ESTABLISHED = 1;
const char* ERROR_COMM_CONNECTION_NOT_ESTABLISHED_MESSAGE = "A control message was received without a successful connection handshake beforehand.";
const int ERROR_REMOTE_CONTROL_TIMEOUT = 2;
const char* ERROR_REMOTE_CONTROL_TIMEOUT_MESSAGE = "While in active remote control mode, no control (or heartbeat) message was received in the expected interval.";
const int ERROR_HARDWARE_NO_THROTTLE_DAC = 3;
const char* ERROR_HARDWARE_NO_THROTTLE_DAC_MESSAGE = "No connection to motor throttle control component (DAC via I2C).";
const int ERROR_MODES_MANUAL_MODE_SWITCH_CONFLICTING = 5;
const char* ERROR_MODES_MANUAL_MODE_SWITCH_CONFLICTING_MESSAGE = "Hardware malfuntion in manual mode switch. Both directions are active.";
const int ERROR_MODES_VEHICLE_MOVING_WHILE_PARKING = 6;
const char* ERROR_MODES_VEHICLE_MOVING_WHILE_PARKING_MESSAGE = "Vehicle is moving while in parking mode.";
const int ERROR_MODES_DIRECTION_CONFLICTS_MOVEMENT = 7;
const char* ERROR_MODES_DIRECTION_CONFLICTS_MOVEMENT_MESSAGE = "Requested movement direction opposes the current movement direction of the vehicle.";
const int ERROR_MODES_DIRECTION_CONFLICTS_UNKNOWN_MOVEMENT = 8;
const char* ERROR_MODES_DIRECTION_CONFLICTS_UNKNOWN_MOVEMENT_MESSAGE = "Requested movement direction may oppose the current movement direction of the vehicle which cannot be determined.";
const int ERROR_MODES_EMERGENCY_NOT_YET_RECOVERABLE = 9;
const char* ERROR_MODES_EMERGENCY_NOT_YET_RECOVERABLE_MESSAGE = "Emergency mode is not yet recoverable. Only requested a driving mode after the error is resolved and the vehicle did not move for the configured time.";

// Mapping of error codes to human-readable messages.
const char* ERROR_MAP[] = {
   "Connection refused due to protocol version mismatch between driver and remote control.",
   "A control message was received without a successful connection handshake beforehand.",
   "While in active remote control mode, no control (or heartbeat) message was received in the expected interval.",
   "No connection to motor throttle control component (DAC via I2C).",
   NULL,
   "Hardware malfuntion in manual mode switch. Both directions are active.",
   "Vehicle is moving while in parking mode.",
   "Requested movement direction opposes the current movement direction of the vehicle.",
   "Requested movement direction may oppose the current movement direction of the vehicle which cannot be determined.",
   "Emergency mode is not yet recoverable. Only requested a driving mode after the error is resolved and the vehicle did not move for the configured time."
};
