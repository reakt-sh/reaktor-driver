#include <Arduino.h>
#include <bitproto.h>
// Generated code library
#include <communication_bp.h>
#include <config_communication.h>

#include "communication.h"

#include "debug.h"
#include "revolutions.h"
#include "speed.h"
#include "mode.h"
#include "error.h"

// Variables
bool hasPeer = false; // Indicates if a remote peer has been detected
unsigned long lastStatusMessageTime = 0; // Timestamp of the last sent status message
unsigned long lastControlMessageTime = 0; // Timestamp of the last received control message

byte controlMessageBuffer[BYTES_LENGTH_CONTROL_MESSAGE]; // Buffer for incoming control message
unsigned int controlMessageBufferPosition = 0; // Current position in the control message buffer

// Current status
struct StatusMessage status = {
    .error = NO_ERROR,
    .remote_control = false,
    .time = 0,
    .mode = NEUTRAL,
    .motor_rpm = 0,
    .target_rpm = 0,
    .control_rpm = 0
};

// ====================
// API

bool setupCommunication() {
    // Setup serial connection
    Serial.begin(COMM_SERIAL_BAUDRATE);

    return true;
}

bool checkRemoteControl(bool remoteControlEnabled, tControlCommand* command) {
    // Note in status whether remote control is active
    status.remote_control = remoteControlEnabled;
    // Check input buffer for data
    // Loop time will usually be faster than serial input, so there won't be much data
    while (Serial.available() > 0) {
        // Read data
        controlMessageBuffer[controlMessageBufferPosition] = (byte) Serial.read();
        controlMessageBufferPosition++;

        // Process data
        if (controlMessageBufferPosition >= BYTES_LENGTH_CONTROL_MESSAGE) {
            hasPeer = true; // A peer is present if any data is received
            controlMessageBufferPosition = 0; // Reset buffer position for next message

            if (remoteControlEnabled) {
                // Update last control message time
                lastControlMessageTime = millis();

                // Decode message
                struct ControlMessage controlMessage;
                DecodeControlMessage(&controlMessage, controlMessageBuffer);

                if (controlMessage.mode == HEARTBEAT) {
                    // Just a keep-alive message, ignore payload
                    return false;
                } else if (controlMessage.mode == UNUSED_B || controlMessage.mode == UNUSED_C) {
                    // Will be special control message, ignore
                    return false;
                } else {
                    // Valid control message
                    command->mode = (DriveMode) controlMessage.mode;
                    command->target_rpm = controlMessage.target_rpm;
                    return true;
                }
            }
        }
    }

    // Check for connection timeout if in remote control mode
    if (remoteControlEnabled && hasPeer && (millis() - lastControlMessageTime > COMM_CONTROL_MESSAGE_HEARTBEAT_TIME)) {
        // No message received in a while, disable remote control and issue emergency stop
        triggerEmergencyStop();
        registerError(ERROR_REMOTE_CONTROL_TIMEOUT);
        hasPeer = false; // Assume peer is gone
    }

    return false;
}

void sendStatusReport(bool force) {
    unsigned long spacing = millis() - lastStatusMessageTime;
    // Send status message if forced or enough time has passed since last message (ten times bigger interval if no peer is yet known)
    if (force || (hasPeer && spacing >= COMM_STATUS_MESSAGE_SPACING_TIME) || (!hasPeer && spacing >= COMM_STATUS_MESSAGE_SPACING_TIME * 10)) {
        // Update status
        updateStatus();

        // Encode message
        byte buffer[BYTES_LENGTH_STATUS_MESSAGE];
        EncodeStatusMessage(&status, buffer);

        // Send message
        Serial.write(buffer, BYTES_LENGTH_STATUS_MESSAGE);
        lastStatusMessageTime = millis();

        if (status.error != NO_ERROR) {
            // Send error appendix message
            struct ErrorAppendixMessage errorMessage;
            errorMessage.errors = consumeErrors();
            byte errorBuffer[BYTES_LENGTH_ERROR_APPENDIX_MESSAGE];
            EncodeErrorAppendixMessage(&errorMessage, errorBuffer);
            Serial.write(errorBuffer, BYTES_LENGTH_ERROR_APPENDIX_MESSAGE);
        }

        Serial.flush(); // Ensure all data is sent
    }
}


// ====================
// INTERNAL

void updateStatus () {
    // Update status values
    if (hasFatalError()) {
        status.error = FATAL_ERROR;
    } else if (hasError()) {
        status.error = ERROR;
    } else {
        status.error = NO_ERROR;
    }
    status.time = millis();
    status.mode = (Mode) getCurrentMode();
    status.motor_rpm = getRevolutionsPerMinute();
    status.target_rpm = getTargetRPM();
    status.control_rpm = getControlRPM();
}
