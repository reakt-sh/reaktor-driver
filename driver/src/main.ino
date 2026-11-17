/*
 * Code for the Reaktor motor driver.
 * It is designed to run on an Arduino Mega controlling the Motor via the Kelly controller.
 */
// Generated code library
#include <config_driver.h>
#include <config_hardware.h>
#include <config_communication.h>

#include "communication.h"
#include "revolutions.h"
#include "lever.h"
#include "motor.h"
#include "speed.h"
#include "switches.h"
#include "panel.h"
#include "safety.h"
#include "error.h"

// ====================
// SETUP

void setup(void) {
    // Setup Handlers
    bool success = true;
    success &= setupSwitches();
    success &= setupRevolutionsDetection();
    success &= setupThrottleLever();
    success &= setupMotorSignals();
    success &= setupSpeedHandler();
    success &= setupSafetyControls();
    success &= setupPanel();
    success &= setupCommunication();

    // Handle fatal setup errors
    if (!success) {
        registerFatalError(ERROR_SETUP_FAILED);
        triggerEmergencyStop();
    }
}


// ====================
// LOOP

// Current remote control command.
// Defined outside of loop to keep value even if no new command is received in a loop iteration.
tControlCommand remoteControl = {
    .mode = PARKING,
    .target_rpm = 0
};

void loop(void) {
    if (hasFatalError()) {
        // In case of fatal error keep in safe state
        setBrakeSignal(true);
        setDirectionSignal(PARKING);
        setMotorThrottle(0);
        // Handle basic communication to allow error reporting
        handleCommunication(false, NULL, NULL);
        sendStatusReport(true);
        // Wait regular response time
        delay(COMM_STATUS_MESSAGE_SPACING_TIME);
    } else {
        // = Main control pipeline =
        // 1. Read hardware inputs
        updateAveragedLeverThrottle();
        int manualTargetSpeedRPM = getAveragedLeverThrottle();
        bool manualSwitchForward = isManualForwardSwitchEngaged();
        bool manualSwitchReverse = isManualReverseSwitchEngaged();
        bool remoteControlRequested = isRemoteControlSwitchEngaged();
        int currentSpeedRPM = getRevolutionsPerMinute();

        // 3. Handle communication and update remote control command if neccessary
        bool newControlReceived = false;
        bool hasConnection = handleCommunication(remoteControlRequested, &remoteControl, &newControlReceived);
        if (!hasConnection) {
            // If no connection is established, reset remote control command to safe state
            remoteControl.mode = PARKING;
            remoteControl.target_rpm = 0;
        }

        // Perform safety checks
        tControlCommand safeControl = { .mode = PARKING, .target_rpm = 0 };
        bool remoteControlActive = false;
        computeSafeControl(
            currentSpeedRPM,
            manualTargetSpeedRPM,
            manualSwitchForward,
            manualSwitchReverse,
            remoteControlRequested,
            hasConnection,
            &remoteControl,
            &safeControl,
            &remoteControlActive
        );

        // Apply motor speed control
        controlSpeed(currentSpeedRPM, &safeControl);

        // Do sanity checks
        checkMotorThrottle();

        // Update information on panel
        updatePanel(currentSpeedRPM, &safeControl, remoteControlActive);

        // Send status report
        sendStatusReport(newControlReceived);
    }
}
