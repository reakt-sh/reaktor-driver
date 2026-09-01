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
#include "panel.h"
#include "mode.h"
#include "error.h"

// ====================
// SETUP

void setup(void) {
    // Setup Handlers
    bool success = true;
    success &= setupSwitches();
    success &= setupRevolutionsDetection();
    success &= setupThrottleHandler();
    success &= setupSpeedHandler();
    success &= setupPanel();
    success &= setupCommunication();

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
            // Manual throttle readings tend to be noisy, so only update target speed if the difference is above a threshold
            // TODO In the future noise elimination should be realized via time-based filtering not a hard rpm limit, especially when the speed limit is increased
            int newTargetSpeed = readManualThrottle();
            if (newTargetSpeed == 0 || abs(newTargetSpeed - getTargetRPM()) >= DRIVER_THROTTLE_READING_REACTION_THRESHOLD_RPM) {
                setTargetRPM(newTargetSpeed);
            }
        }

        // Apply motor speed control
        controlSpeed();

        // Do sanity checks
        checkMotorThrottle();

        // Update information on panel
        updatePanel();

        // Send status report
        sendStatusReport(newControl);
    }
}
