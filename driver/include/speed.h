#pragma once

/**
 * Setup the speed control handler.
 *
 * @return true if the setup was successful
 */
bool setupSpeedHandler();

/**
 * Set the target RPM for the motor.
 */
void setTargetRPM(unsigned int rpm);

/**
 * Get the current target RPM.
 */
unsigned int getTargetRPM();

/**
 * Get the current RPM of the motor.
 * This value is only up-to-date after calling controlSpeed().
 */
unsigned int getCurrentRPM();

/**
 * Get the currently applied RPM.
 */
unsigned int getControlRPM();
/**
 * Control motor speed.
 */
void controlSpeed();

