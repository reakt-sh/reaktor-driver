#include <Arduino.h>
#include <Wire.h>
#include <Adafruit_MCP4725.h>
// Generated code library
#include <config_hardware.h>

#include "throttle.h"

#include "debug.h"
#include "error.h"
#include "mode.h"

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
// Throttle lever deadzones
const int MANUAL_THROTTLE_LOWER_DEADZONE_LIMIT = THROTTLE_LEVER_LOWER_DEADZONE * ADC_STEPS; // Lower deadzone limit in ADC steps
const int MANUAL_THROTTLE_UPPER_DEADZONE_LIMIT = ADC_STEPS - (THROTTLE_LEVER_UPPER_DEADZONE * ADC_STEPS); // Upper deadzone limit in ADC steps
const int MANUAL_THROTTLE_EFFECTIVE_RANGE = MANUAL_THROTTLE_UPPER_DEADZONE_LIMIT - MANUAL_THROTTLE_LOWER_DEADZONE_LIMIT; // Effective range in ADC steps

// Variables
int lastSetRPM = 0;

// Hardware components
Adafruit_MCP4725 dac;

// ====================
// API

bool setupThrottleHandler() {
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
        lastSetRPM = rpm;
        // Convert to DAC scale value
        int value = RPM_CONVERSION_FACTOR * rpm;
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

int readManualThrottle() {
    int sample = analogRead(BOARD_PIN_IN_MANUAL_THROTTLE); // Read the analog input
    // Apply deadzone
    if (sample <= MANUAL_THROTTLE_LOWER_DEADZONE_LIMIT) {
        return 0;
    } else if (sample >= MANUAL_THROTTLE_UPPER_DEADZONE_LIMIT) {
        return MOTOR_MAX_RPM;
    } else {
        // Convert to relative position (0.0 - 1.0) in deadzone-free area
        float relative_pos = ((float) sample) / MANUAL_THROTTLE_EFFECTIVE_RANGE;
        // Linear mapping to RPM
        int rpm = relative_pos * MOTOR_MAX_RPM;

        return rpm;
    }
}


// ====================
// INTERNAL
