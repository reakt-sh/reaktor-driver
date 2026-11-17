#include <Arduino.h>
#include <Wire.h>
#include <Adafruit_MCP4725.h>
// Generated code library
#include <config_hardware.h>

#include "motor.h"

#include "safety.h"
#include "error.h"

// Configuration
/** Address on on I2C bus
 * For Adafruit MCP4725A1 the address is 0x62 (default) or 0x63 (ADDR pin tied to VCC)
 * For MCP4725A0 the address is 0x60 or 0x61
 * For MCP4725A2 the address is 0x64 or 0x65
 */
const uint8_t DAC_ADDRESS = 0x62;
// Conversion factor from RPM to DAC value
// Assumes linear relation between voltage and motor speed, which is not fully the case
const double RPM_CONVERSION_FACTOR = ((double) DAC_STEPS) / MOTOR_MAX_RPM;

// Variables
unsigned int lastSetRPM = 0;

// Hardware components
Adafruit_MCP4725 dac;

// ====================
// API

bool setupMotorSignals() {
    // Configure digital IO pins
    pinMode(BOARD_PIN_OUT_MOTOR_FORWARD, OUTPUT);
    pinMode(BOARD_PIN_OUT_MOTOR_REVERSE, OUTPUT);
    pinMode(BOARD_PIN_OUT_MOTOR_BRAKE, OUTPUT);
    pinMode(BOARD_PIN_OUT_MOTOR_REGENERATIVE, OUTPUT);

    // Set initial output states
    digitalWrite(BOARD_PIN_OUT_MOTOR_FORWARD, LOW);
    digitalWrite(BOARD_PIN_OUT_MOTOR_REVERSE, LOW);
    digitalWrite(BOARD_PIN_OUT_MOTOR_BRAKE, LOW);
    digitalWrite(BOARD_PIN_OUT_MOTOR_REGENERATIVE, LOW);

    // Connect to device
    bool success = dac.begin(DAC_ADDRESS);

    if (success) {
        // Initialize to zero and save in EPROM
        dac.setVoltage(0, true);
    } else {
        registerFatalError(ERROR_HARDWARE_NO_THROTTLE_DAC);
    }
    return success;
}

void setMotorThrottle(int rpm) {
    if (hasFatalError()) {
        // In case of fatal error, ensure throttle is zero
        rpm = 0;
    }
    if (rpm != lastSetRPM) {
        lastSetRPM = rpm < 0 ? 0 : rpm; // Ensure non-negative
        // Convert to DAC scale value
        int value = RPM_CONVERSION_FACTOR * min(lastSetRPM, MOTOR_MAX_RPM);
        // Limit to valid range
        value = min(max(value, 0), DAC_STEPS);
        // Send to DAC
        bool success = dac.setVoltage(value, false);
        if (!success) {
            registerFatalError(ERROR_HARDWARE_NO_THROTTLE_DAC);
            triggerEmergencyStop();
        }
    }
}

void checkMotorThrottle() {
    // int sample = analogRead(BOARD_PIN_IN_DAC); // Read the analog input
    // float voltage = (((float) sample) / ADC_STEPS) * V_REF;  // Convert to voltage
    // TODO Check if DAC produces the correct voltage (in time) and raise error if not, also stop vehicle
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

// ====================
// INTERNAL
