#include <Arduino.h>
// Generated code library
#include <config_hardware.h>

#include "switches.h"

// ====================
// API

bool setupSwitches() {
    // Configure digital IO pins
    pinMode(BOARD_PIN_IN_ONBOARD, INPUT_PULLUP);
    pinMode(BOARD_PIN_IN_ENABLE_REMOTE_CONTROL, INPUT_PULLUP);
    pinMode(BOARD_PIN_IN_MANUAL_FORWARD, INPUT_PULLUP);
    pinMode(BOARD_PIN_IN_MANUAL_REVERSE, INPUT_PULLUP);

    return true;
}

bool isRemoteControlSwitchEngaged() {
    return digitalRead(BOARD_PIN_IN_ENABLE_REMOTE_CONTROL) == LOW;
}

bool isManualForwardSwitchEngaged() {
    return digitalRead(BOARD_PIN_IN_MANUAL_FORWARD) == LOW;
}

bool isManualReverseSwitchEngaged() {
    return digitalRead(BOARD_PIN_IN_MANUAL_REVERSE) == LOW;
}
