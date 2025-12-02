#include <Arduino.h>
// Generated code library
#include <config_hardware.h>
#include <config_driver.h>

#include "revolutions.h"

#include "debug.h"

// Variables
volatile unsigned long revolutionDuration = 0; // Milliseconds of the last motor revolution
volatile unsigned long lastEdgeTime = 0; // Milliseconds since last signal edge
volatile unsigned long lastRevolutionTime = 0; // Milliseconds since last revolution
volatile uint8_t edgeCounter = 0; // Number of edges since last revolution

// ====================
// API

bool setupRevolutionsDetection() {
    // Attach to rising edge of meter signal which is a copy of hall sensor A on the motor
    attachInterrupt(digitalPinToInterrupt(BOARD_PIN_IN_MOTOR_METER), _readMeterPWM, RISING);

    return true;
}

int getRevolutionsPerMinute() {
    const unsigned long currentTime = millis();
    // Since we store the duration of the last full revolution, we must consider that more time passes when speed is reducing
    if (lastRevolutionTime == 0 || revolutionDuration == 0) {
        // No revolution has been measured yet
        return 0;
    } else if (currentTime < lastRevolutionTime + revolutionDuration) {
        // Time is within the next expected revolution, hence we return the last measured one
        return 60000.0 / revolutionDuration;
    } else {
        // We exceeded the expected revolution, hence the motor has stopped or is slowing down
        const unsigned long currentRevolutionDuration = currentTime - lastRevolutionTime;
        const unsigned int currentRPM = 60000.0 / currentRevolutionDuration;
        if (currentRPM >= MOTOR_MIN_RPM) {
            return currentRPM;
        } else {
            // The motor is unable to reach a speed so low, hence we assume a stop
            return 0;
        }
    }
}

// ====================
// INTERNAL

/**
 * ISR for processing meter signal.
 *
 * This code assumes the program will never run longer than 50 days,
 * otherwise the overflow in millis() will break the behavior.
 */
void _readMeterPWM() {
    const unsigned long currentTime = millis();
    const unsigned long edgeDuration = currentTime - lastEdgeTime;
    if (edgeDuration > DRIVER_REVOLUTION_SIGNAL_BLIND_TIME) { // Ignore super fast edges
        lastEdgeTime = currentTime;
        edgeCounter++;
        if (edgeCounter == MOTOR_SIGNAL_EDGES_PER_REVOLUTION) {
            // Only measure time between full revolution to reduce the impact of errors due to millisecond measurement.
            // TODO Check use of micros() but consider erratic behavior mentioned here: https://docs.arduino.cc/language-reference/en/functions/external-interrupts/attachInterrupt/#notes-and-warnings
            revolutionDuration = currentTime - lastRevolutionTime;
            lastRevolutionTime = currentTime;
            edgeCounter = 0;
        }
    }
}
