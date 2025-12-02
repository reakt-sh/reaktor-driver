#include <Arduino.h>
#include <PID_v1.h>
// Generated code library
#include <config_hardware.h>
#include <config_driver.h>

#include "speed.h"

#include "debug.h"
#include "revolutions.h"
#include "throttle.h"
#include "mode.h"
#include "switches.h"

// Configuration
const int PID_SAMPLE_TIME = 25; // PID calculation interval in ms
// Variables
int targetRPM = 0;   // Target speed in RPM
int appliedRPM = 0;  // Applied speed in RPM in last control cycle
bool keepZero = true; // Keep speed at 0 RPM until first non-zero target is set

// PID Controller
double pidSetpoint, pidInput, pidOutput;
const double pidKp = 1, pidKi = 1, pidKd = 0;
// PID logic
// Link to IO variables, apply tuning parameters, activate proportional-on-measurement, set to non-reversed (direct) mode
// For P-on-M see: http://brettbeauregard.com/blog/2017/06/introducing-proportional-on-measurement/
PID pidLogic(&pidInput, &pidOutput, &pidSetpoint, pidKp, pidKi, pidKd, P_ON_M, DIRECT);

// ====================
// API

bool setupSpeedHandler() {
    // Configure and start PID controller
    pidLogic.SetOutputLimits(0, MOTOR_MAX_RPM);
    pidLogic.SetSampleTime(PID_SAMPLE_TIME);
    pidLogic.SetMode(keepZero ? MANUAL : AUTOMATIC); // Start in manual mode, i.e. no control

    return true;
}

void setTargetRPM(int rpm) {
    targetRPM = rpm;
}

int getTargetRPM() {
    return targetRPM;
}

int getControlRPM() {
    return appliedRPM;
}

void controlSpeed() {
    int currentRPM = getRevolutionsPerMinute();
    int mode = getCurrentMode();

    // 1. Handle braking
    if (mode == PARKING || mode == EMERGENCY_STOP) {
        // Always brake in parking or emergency stop mode
        setBrakeSignal(true);
    } else if (mode == DRIVE_FORWARD || mode == DRIVE_REVERSE) {
        if (targetRPM > 0) {
            if (currentRPM - targetRPM > DRIVER_BRAKING_THRESHOLD_RPM) {
                // Brake if targeted deceleration exceeds braking threshold
                setBrakeSignal(true);
            } else {
                // Release brake in case no braking is needed or target speed is nearly reached
                setBrakeSignal(false);
            }
        } else {
            // Try to kep the vehicle at a standstill
            setBrakeSignal(true);
        }
    } else {
        // In all other cases, release brake
        setBrakeSignal(false);
    }

    // 2. Handle speed via pid controller

    // Due to deadzone in the motor, the PID controller would sometimes restarts the motor after being stopped
    // This code ensures that the motor is kept at 0 RPM until a non-zero target speed is set
    if (keepZero && targetRPM > 0) {
        // Activate PID control when first non-zero target is set
        pidLogic.SetMode(AUTOMATIC);
        keepZero = false;
    } else if (!keepZero && targetRPM == 0 && currentRPM == 0) {
        // Deactivate PID control when target and current speed are zero
        pidLogic.SetMode(MANUAL);
        keepZero = true;
    }

    // Set PID target
    pidSetpoint = targetRPM;
    // Set PID feedback input from motor
    pidInput = currentRPM;

    // Run PID control logic
    pidLogic.Compute();

    // Set motor speed via throttle
    if (keepZero) {
        appliedRPM = 0; // Override PID output to ensure 0 RPM
    } else {
        appliedRPM = (int) pidOutput;
    }
    setMotorThrottle(appliedRPM);
}

// ====================
// INTERNAL
