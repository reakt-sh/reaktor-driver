#include <Arduino.h>
// Generated code library
#include <config_hardware.h>

#include "switches.h"

#include "debug.h"

// ====================
// API

bool setupSwitches() {
    // Configure digital IO pins
    pinMode(BOARD_PIN_IN_ONBOARD_ENABLE_REMOTE_CONTROL, INPUT_PULLUP);
    pinMode(BOARD_PIN_IN_MANUAL_FORWARD, INPUT_PULLUP);
    pinMode(BOARD_PIN_IN_MANUAL_REVERSE, INPUT_PULLUP);
    pinMode(BOARD_PIN_OUT_MOTOR_FORWARD, OUTPUT);
    pinMode(BOARD_PIN_OUT_MOTOR_REVERSE, OUTPUT);
    pinMode(BOARD_PIN_OUT_MOTOR_BRAKE, OUTPUT);
    pinMode(BOARD_PIN_OUT_MOTOR_REGENERATIVE, OUTPUT);

    // Set initial output states
    digitalWrite(BOARD_PIN_OUT_MOTOR_FORWARD, LOW);
    digitalWrite(BOARD_PIN_OUT_MOTOR_REVERSE, LOW);
    digitalWrite(BOARD_PIN_OUT_MOTOR_BRAKE, LOW);
    digitalWrite(BOARD_PIN_OUT_MOTOR_REGENERATIVE, LOW);

    return true;
}

bool isRemoteControlSwitchEngaged() {
    return digitalRead(BOARD_PIN_IN_ONBOARD_ENABLE_REMOTE_CONTROL) == LOW;
}

bool isManualForwardSwitchEngaged() {
    return digitalRead(BOARD_PIN_IN_MANUAL_FORWARD) == LOW;
}

bool isManualReverseSwitchEngaged() {
    return digitalRead(BOARD_PIN_IN_MANUAL_REVERSE) == LOW;
}

void setBrakeSignal(bool engaged) {
    digitalWrite(BOARD_PIN_OUT_MOTOR_BRAKE, engaged ? HIGH : LOW);
}

void setDirectionSignal(tDriveMode mode) {
    switch (mode) {
        case DRIVE_FORWARD:
            digitalWrite(BOARD_PIN_OUT_MOTOR_FORWARD, HIGH);
            digitalWrite(BOARD_PIN_OUT_MOTOR_REVERSE, LOW);
            break;
        case DRIVE_REVERSE:
            digitalWrite(BOARD_PIN_OUT_MOTOR_FORWARD, LOW);
            digitalWrite(BOARD_PIN_OUT_MOTOR_REVERSE, HIGH);
            break;
        default: // all other modes
            digitalWrite(BOARD_PIN_OUT_MOTOR_FORWARD, LOW);
            digitalWrite(BOARD_PIN_OUT_MOTOR_REVERSE, LOW);
            break;
    }
    // FIXME just for indicating as long we dont have a status LED:
    digitalWrite(BOARD_PIN_OUT_MOTOR_REGENERATIVE, mode == EMERGENCY_STOP ? HIGH : LOW);
}
