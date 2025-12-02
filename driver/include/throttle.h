#pragma once

/**
 * Setup the throttle handler.
 *
 * @return true if the setup was successful
 */
bool setupThrottleHandler();

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
 * Reads the throttle lever value.
 * This is a mapping of the lever position to rpm.
 *
 * @retrun The desired rpm as set by the throttle lever
 */
int readManualThrottle();
