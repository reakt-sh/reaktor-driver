#pragma once

#include <modes.h>

/**
 * Setup the motor signals.
 *
 * @return true if the setup was successful
 */
bool setupMotorSignals();

/**
 * Sends the given speed in rpm as throttle to the motor controller.
 * The motor controller will interpret the value somehow differently resulting in only approximately the given speed on the motor.
 * Use an appropriate control logic to achieve the desired speed.
 *
 * @param rpm The throttle value in rpm
 */
void setMotorThrottle(int rpm);

/**
 * Check if the motor throttle signal is correctly applied.
 */
void checkMotorThrottle();

/**
 * Set the brake signal to the given state.
 *
 * @param engaged true to engage the brake, false to release
 */
void setBrakeSignal(bool engaged);

/**
 * Set the direction signal according to the given drive mode.
 *
 * @param mode the drive mode to set the signal for
 */
void setDirectionSignal(tDriveMode mode);
