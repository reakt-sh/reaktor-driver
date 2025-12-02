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
void setTargetRPM(int rpm);

/**
 * Get the current target RPM.
 */
int getTargetRPM();

/**
 * Get the currently applied RPM.
 */
int getControlRPM();
/**
 * Control motor speed.
 */
void controlSpeed();

