#include <Arduino.h>

#include "mode.h"

#include "switches.h"
#include "speed.h"
#include "debug.h"

// ====================
// API

tDriveMode currentMode = NEUTRAL;

bool isControlModeChangeSafe() {
    // FIXME Implement
    return true;
}

tDriveMode inferModeFromSwitches() {
    // FIXME Trigger emergency stop when both switches are on!
    if (isManualForwardSwitchEngaged()) {
        return DRIVE_FORWARD;
    } else if (isManualReverseSwitchEngaged()) {
        return DRIVE_REVERSE;
    } else {
        return NEUTRAL;
    }
}

void setNextMode(tDriveMode mode) {
    // TODO Implement mode safety checks
    // Just for testing
    setDirectionSignal(mode);
    currentMode = mode;
}

tDriveMode getCurrentMode() {
    return currentMode;
}

void triggerEmergencyStop() {
    if (currentMode != EMERGENCY_STOP) {
        setNextMode(EMERGENCY_STOP);
        setTargetRPM(0);
    }
}
