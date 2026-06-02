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
bool isConnected = false; // Indicates if a remote peer has been detected
unsigned long lastStatusMessageTime = 0; // Timestamp of the last sent status message
unsigned long lastControlMessageTime = 0; // Timestamp of the last received control message
unsigned int controlAcknowledgement = 0; // Acknowledgement code of last control message

byte messageBuffer[max(BYTES_LENGTH_CONTROL_MESSAGE, max(BYTES_LENGTH_CONNECT_APPENDIX_MESSAGE, BYTES_LENGTH_CONFIGURATION_APPENDIX_MESSAGE))]; // Buffer for incoming message
unsigned int messageBufferPosition = 0; // Current position in the message buffer
enum class ExpectedMessage {
    CONTROL_MESSAGE,
    CONNECT_APPENDIX_MESSAGE,
    CONFIGURATION_APPENDIX_MESSAGE
} expectedMessage = ExpectedMessage::CONTROL_MESSAGE; // Type of message we are currently expecting
// VS Code shows an error on this line, which is a false-positive (https://github.com/microsoft/vscode-cpptools/issues/8175)

// Current status
struct StatusMessage status = {
    .error = NO_ERROR,
    .remote_control = false,
    .time = 0,
    .connection_established = false,
    .control_acknowledgement = 0,
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

bool handleCommunication(bool remoteControlEnabled, tControlCommand* command) {
    // Note in status whether remote control is active
    status.remote_control = remoteControlEnabled;
    // Check input buffer for data
    // Loop time will usually be faster than serial input, so there won't be much data
    while (Serial.available() > 0) {
        // Read data
        messageBuffer[messageBufferPosition] = (byte) Serial.read();
        messageBufferPosition++;

        // Process data
        // Only process one message at a time, to ensure each message is acknowledged
        switch (expectedMessage) {
            case ExpectedMessage::CONTROL_MESSAGE:
                if (messageBufferPosition >= BYTES_LENGTH_CONTROL_MESSAGE) {
                    bool control = processControlMessage(remoteControlEnabled, command);

                    messageBufferPosition = 0; // Reset buffer position for next message
                    return control;
                }
                break;
            case ExpectedMessage::CONNECT_APPENDIX_MESSAGE:
                if (messageBufferPosition >= BYTES_LENGTH_CONNECT_APPENDIX_MESSAGE) {
                    processConnectMessage();

                    expectedMessage = ExpectedMessage::CONTROL_MESSAGE; // Reset expected message type
                    messageBufferPosition = 0; // Reset buffer position for next message
                    return false;
                }
                break;
            case ExpectedMessage::CONFIGURATION_APPENDIX_MESSAGE:
                if (messageBufferPosition >= BYTES_LENGTH_CONFIGURATION_APPENDIX_MESSAGE) {
                    processConfigurationMessage();

                    expectedMessage = ExpectedMessage::CONTROL_MESSAGE; // Reset expected message type
                    messageBufferPosition = 0; // Reset buffer position for next message
                    return false;
                }
                break;
        }
    }

    // Check for connection timeout if in remote control mode
    if (remoteControlEnabled && isConnected && (millis() - lastControlMessageTime > COMM_CONTROL_MESSAGE_HEARTBEAT_TIME)) {
        // No message received in a while, disable remote control and issue emergency stop
        triggerEmergencyStop();
        registerError(ERROR_REMOTE_CONTROL_TIMEOUT);
        isConnected = false; // Assume peer is gone
    }

    return false;
}

void sendStatusReport(bool force) {
    unsigned long spacing = millis() - lastStatusMessageTime;
    // Send status message if forced or enough time has passed since last message (ten times bigger interval if no peer is yet known)
    if (force || controlAcknowledgement > 0 || (isConnected && spacing >= COMM_STATUS_MESSAGE_SPACING_TIME) || (!isConnected && spacing >= COMM_STATUS_MESSAGE_SPACING_TIME * 10)) {
        // Update status
        updateStatus();

        // Encode message
        byte buffer[BYTES_LENGTH_STATUS_MESSAGE];
        EncodeStatusMessage(&status, buffer);

        // Send message
        Serial.write(buffer, BYTES_LENGTH_STATUS_MESSAGE);
        lastStatusMessageTime = millis();
        controlAcknowledgement = 0; // Reset acknowledgement after sending

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

/**
 * Processes an incoming control message.
 * If in remote control mode and a valid control message is received, updates the command structure.
 *
 * @param remoteControlEnabled Indicates if remote control mode is enabled.
 * @param command Pointer to the control command structure to be updated.
 *
 * @returns true if a valid control command was processed, false otherwise.
 */
bool processControlMessage(bool remoteControlEnabled, tControlCommand* command) {
    if (messageBufferPosition >= BYTES_LENGTH_CONTROL_MESSAGE) {
        // Update last control message time
        lastControlMessageTime = millis();

        // Decode message
        struct ControlMessage controlMessage;
        DecodeControlMessage(&controlMessage, messageBuffer);

        // Store acknowledgement code
        controlAcknowledgement = controlMessage.acknowledge;

        if (controlMessage.mode == HEARTBEAT) {
            // Just a keep-alive message, ignore payload
            return false;
        } else if (controlMessage.mode == CONNECT) {
            // Switch to expecting connect appendix message
            expectedMessage = ExpectedMessage::CONNECT_APPENDIX_MESSAGE;
            return false;
        } else if (!isConnected) {
            registerError(ERROR_COMM_CONNECTION_NOT_ESTABLISHED);
            return false;
        } else if (controlMessage.mode == CONFIG) {
            // Switch to expecting configuration appendix message
            expectedMessage = ExpectedMessage::CONFIGURATION_APPENDIX_MESSAGE;
            return false;
        } else if (remoteControlEnabled) {
            // Actual remote control message
            command->mode = (DriveMode) controlMessage.mode;
            command->target_rpm = controlMessage.target_rpm;
            return true;
        }
    }
    return false;
}

/**
 * Processes an incoming connect appendix message.
 * Validates the protocol version and registers an error if there is a mismatch.
 */
void processConnectMessage() {
    // Decode message
    struct ConnectAppendixMessage connectMessage;
    DecodeConnectAppendixMessage(&connectMessage, messageBuffer);

    // Store acknowledgement code
    controlAcknowledgement = connectMessage.acknowledge;

    // Check protocol version
    if (connectMessage.protocol_version != COMM_PROTOCOL_VERSION) {
        registerError(ERROR_COMM_CONNECTION_VERSION_MISMATCH);
    } else {
        // Protocol version matches, connection is valid
        isConnected = true;
    }
}

/**
 * Processes an incoming configuration appendix message.
 */
void processConfigurationMessage() {
    // Decode message
    struct ConfigurationAppendixMessage configMessage;
    DecodeConfigurationAppendixMessage(&configMessage, messageBuffer);

    // Store acknowledgement code
    controlAcknowledgement = configMessage.acknowledge;

    // TODO apply configuration
}

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
    status.connection_established = isConnected;
    status.control_acknowledgement = controlAcknowledgement;
    status.mode = (Mode) getCurrentMode();
    status.motor_rpm = getRevolutionsPerMinute();
    status.target_rpm = getTargetRPM();
    status.control_rpm = getControlRPM();
}
