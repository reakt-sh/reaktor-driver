#include <Arduino.h>

// Generated code library
#include <kieler-gen/SafeDriveModeControl.h>
#include <config_driver.h>
#include <errors.h>

#include "safety.h"

#include "error.h"
#include "communication.h"


// Variables
TickData safetyAutomata;
unsigned long lastTickTime = 0;
bool issueEmergencyStop = false;

// ====================
// API

bool setupSafetyControls() {
    // Initialize the SCChart
    reset(&safetyAutomata);

    // Set constant inputs once
    safetyAutomata.MOVEMENT_THRESHOLD_RPM = DRIVER_MODES_MOVEMENT_THRESHOLD_RPM;
    safetyAutomata.RECOVERY_TIME = DRIVER_MODES_EMERGENCY_RECOVERY_TIME_MS;

    return true;
}

void computeSafeControl(int currentSpeedRPM, int manualTargetSpeedRPM, bool manualSwitchForward, bool manualSwitchReverse, bool remoteControlRequested, bool remoteControlConnected, tControlCommand* remoteControl, tControlCommand* safeControl, bool* remoteControlActivated) {
    // Update inputs
    safetyAutomata.currentSpeedRPM = currentSpeedRPM;
    safetyAutomata.manualSwitchForward = manualSwitchForward;
    safetyAutomata.manualSwitchReverse = manualSwitchReverse;
    safetyAutomata.manualTargetSpeedRPM = manualTargetSpeedRPM;
    safetyAutomata.manualSwitchRemoteControl = remoteControlRequested;
    safetyAutomata.remoteControlConnected = remoteControlConnected;
    safetyAutomata.remoteDriveMode = remoteControl->mode;
    safetyAutomata.remoteTargetSpeedRPM = remoteControl->target_rpm;
    safetyAutomata.issueEmergencyStop = issueEmergencyStop;

    // Reset trigger flag
    issueEmergencyStop = false;

    // Pass time
    unsigned long currentTime = millis();
    safetyAutomata.deltaT = currentTime < lastTickTime ? currentTime : currentTime - lastTickTime; // This computes the time difference and handles overflow but with imprecise results because usually the time difference is very small.
    lastTickTime = currentTime;

    // Execute SCChart tick
    tick(&safetyAutomata);

    // Apply outputs
    safeControl->mode = safetyAutomata.safeDriveMode;
    safeControl->target_rpm = safetyAutomata.safeTargetSpeedRPM;
    *remoteControlActivated = safetyAutomata.remoteControlActive;

    // Handle error flags
    if (safetyAutomata.errorConflictingModeSwitch) {
        registerError(ERROR_MODES_MANUAL_MODE_SWITCH_CONFLICTING);
    }
    if (safetyAutomata.errorMovingWhileParking) {
        registerError(ERROR_MODES_VEHICLE_MOVING_WHILE_PARKING);
    }
    if (safetyAutomata.errorDirectionConflictsMovement) {
        registerError(ERROR_MODES_DIRECTION_CONFLICTS_MOVEMENT);
    }
    if (safetyAutomata.errorDirectionConflictsUnknownMovement) {
        registerError(ERROR_MODES_DIRECTION_CONFLICTS_UNKNOWN_MOVEMENT);
    }
    if (safetyAutomata.errorEmergencyNotYetRecoverable) {
        registerError(ERROR_MODES_EMERGENCY_NOT_YET_RECOVERABLE);
    }

    // Update the status record that will be sent to the remote control
    tStatusMessage* status = getNextStatusReport();
    status->motor_rpm = currentSpeedRPM;
    status->mode = safetyAutomata.safeDriveMode;
    status->target_rpm = safetyAutomata.safeTargetSpeedRPM;
    status->remote_control = safetyAutomata.remoteControlActive;
    if (safetyAutomata.isMoving) {
        if (safetyAutomata.isMovingForward) {
            status->detected_movement = MOVEMENT_FORWARD;
        } else if (safetyAutomata.isMovingReverse) {
            status->detected_movement = MOVEMENT_REVERSE;
        } else {
            status->detected_movement = MOVEMENT_UNKNOWN_DIRECTION;
        }
    } else {
        status->detected_movement = NO_MOVEMENT;
    }
    safetyAutomata.manualSwitchForward = manualSwitchForward;
    safetyAutomata.manualSwitchReverse = manualSwitchReverse;
    safetyAutomata.manualTargetSpeedRPM = manualTargetSpeedRPM;
}

void triggerEmergencyStop() {
    issueEmergencyStop = true;
}
