#pragma once

#include "control_command.h"

typedef struct StatusMessage tStatusMessage;

/**
 * Setup serial communication.
 *
 * @return true if the setup was successful
 */
bool setupCommunication();

/**
 * Handle communication and incoming remote control command.
 *
 * @param remoteControlRequested if true, the message will be discarded and not processed, if false connection loss will cause an error
 * @param control pointer to store the decoded control command
 * @param newControlReceived pointer to a boolean that will be set to true if a new control command was received
 * @return true if a valid control input was received and decoded
 */
bool handleCommunication(bool remoteControlRequested, tControlCommand* control, bool* newControlReceived);

/**
 * Handle sending status report message.
 *
 * @param force if true, the message will be sent immediately, ignoring spacing constraints
 */
void sendStatusReport(bool force);

/**
 * Get the next status report to fill in status information.
 *
 * @return pointer to the next status report
 */
tStatusMessage* getNextStatusReport();
