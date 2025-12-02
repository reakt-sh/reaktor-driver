#pragma once

#include "mode.h"

/**
 * A single remote control command.
 * Type adjusted content of a valid control message.
 */
struct ControlCommand {
    DriveMode mode;
    unsigned int target_rpm;
};
typedef struct ControlCommand tControlCommand;

/**
 * Setup serial communication.
 *
 * @return true if the setup was successful
 */
bool setupCommunication();

/**
 * Handle incoming remote control command.
 *
 * @param ignore if true, the message will be discarded and not processed
 * @param control pointer to store the decoded control command
 * @return true if a valid control input was received and decoded
 */
bool checkRemoteControl(bool ignore, tControlCommand* control);

/**
 * Handle sending status report message.
 *
 * @param force if true, the message will be sent immediately, ignoring spacing constraints
 */
void sendStatusReport(bool force);
