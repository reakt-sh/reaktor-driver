#include <Arduino.h>
// Generated code library
#include <config_hardware.h>
#include <config_driver.h>

#include "lever.h"

// Variables
int lastAveragedLeverThrottle = 0;
uint16_t leverThrottleRingBuffer[DRIVER_THROTTLE_LEVER_MOVING_AVERAGE_WINDOW_SIZE] = {0}; // Array to hold the last N samples of the throttle lever
int leverThrottleRingBufferIndex = 0; // Index for the ring buffer
uint64_t leverThrottleRingBufferSum = 0; // Sum of samples in the ring buffer
// Throttle lever deadzones
const int MANUAL_THROTTLE_LOWER_DEADZONE_LIMIT = THROTTLE_LEVER_LOWER_DEADZONE * ADC_STEPS; // Lower deadzone limit in ADC steps
const int MANUAL_THROTTLE_UPPER_DEADZONE_LIMIT = ADC_STEPS - (THROTTLE_LEVER_UPPER_DEADZONE * ADC_STEPS); // Upper deadzone limit in ADC steps
const int MANUAL_THROTTLE_EFFECTIVE_RANGE = MANUAL_THROTTLE_UPPER_DEADZONE_LIMIT - MANUAL_THROTTLE_LOWER_DEADZONE_LIMIT; // Effective range in ADC steps

// ====================
// API

bool setupThrottleLever() {
    // Initialize ring buffer
    for (int i = 0; i < DRIVER_THROTTLE_LEVER_MOVING_AVERAGE_WINDOW_SIZE; i++) {
        leverThrottleRingBuffer[i] = 0;
    }

    return true;
}

int updateAveragedLeverThrottle() {
    int newValue = readCurrentLeverThrottle();
    // Update ring buffer
    leverThrottleRingBufferSum -= leverThrottleRingBuffer[leverThrottleRingBufferIndex]; // Remove the oldest value from the sum
    leverThrottleRingBuffer[leverThrottleRingBufferIndex] = newValue; // Add the new value to the buffer
    leverThrottleRingBufferSum += newValue; // Add the new value to the sum
    leverThrottleRingBufferIndex = (leverThrottleRingBufferIndex + 1) % DRIVER_THROTTLE_LEVER_MOVING_AVERAGE_WINDOW_SIZE; // Move to the next index in the ring buffer
    // Calculate the average
    int newAverage = leverThrottleRingBufferSum / DRIVER_THROTTLE_LEVER_MOVING_AVERAGE_WINDOW_SIZE;
    if (abs(newAverage - lastAveragedLeverThrottle) > DRIVER_THROTTLE_LEVER_GRANULARITY) {
        // If the change is significant enough, update the last averaged value
        lastAveragedLeverThrottle = newAverage;
    }
    return lastAveragedLeverThrottle;
}

int getAveragedLeverThrottle() {
    return lastAveragedLeverThrottle;
}

int readCurrentLeverThrottle() {
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
