/*
 * Code for the Reaktor motor driver.
 * It is designed to run on an Arduino Mega controlling the Motor via the Kelly controller.
 */
// Generated code library
#include <config_driver.h>
#include <config_hardware.h>
#include <config_communication.h>

#include "debug.h"
#include "communication.h"
#include "revolutions.h"
#include "throttle.h"
#include "speed.h"
#include "switches.h"
#include "mode.h"
#include "error.h"

// ====================
// SETUP

void setup(void) {
    // Setup Handlers
    bool success = true;
    success &= setupCommunication();
    success &= setupSwitches();
    success &= setupRevolutionsDetection();
    success &= setupThrottleHandler();
    success &= setupSpeedHandler();

    // Handle fatal setup errors
    if (!success) {
        triggerEmergencyStop();
    }
}


// ====================
// LOOP

bool remoteControlEnabled = false;
tControlCommand remoteControl;

void loop(void) {
    if (hasFatalError()) {
        // In case of fatal error keep in save state
        triggerEmergencyStop();
        // Handle basic communication to allow error reporting
        handleCommunication(false, &remoteControl);
        sendStatusReport(true);
        // Wait regular response time
        delay(COMM_STATUS_MESSAGE_SPACING_TIME);
    } else {
        // Normal operation
        if (remoteControlEnabled != isRemoteControlSwitchEngaged()) {
            if (isControlModeChangeSafe()) {
                remoteControlEnabled = isRemoteControlSwitchEngaged();
                // Should be ensured by isControlModeChangeSafe
                setNextMode(NEUTRAL);
                setTargetRPM(0);
            } else {
                registerError(ERROR_CONTROL_MODE_SWITCH_UNSAFE);
                triggerEmergencyStop();
            }
        }

        // Handle communication and update control command if new command received
        bool newControl = handleCommunication(remoteControlEnabled, &remoteControl);
        if (remoteControlEnabled) {
            if (newControl) {
                setNextMode(remoteControl.mode);
                setTargetRPM(remoteControl.target_rpm);
            }
        } else {
            // Manual control
            setNextMode(inferModeFromSwitches());
            setTargetRPM(readManualThrottle());
        }

        // Apply motor speed control
        controlSpeed();

        // Do sanity checks
        checkMotorThrottle();

        // Send status report
        sendStatusReport(newControl);
    }
}
