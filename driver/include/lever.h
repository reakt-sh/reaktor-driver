#pragma once

/**
 * Setup the throttle lever handler.
 *
 * @return true if the setup was successful
 */
bool setupThrottleLever();

/**
 * Update averaged throttle value.
 * Call only once per control cycle to avoid giving more weight to the current sample.
 *
 * @return The averaged throttle value in rpm
 */
int updateAveragedLeverThrottle();

/**
 * Returns the averaged throttle value.
 * Does not update the value, only returns the last calculated value.
 *
 * @return The averaged throttle value in rpm
 */
int getAveragedLeverThrottle();

/**
 * Reads the throttle lever value.
 * This is a mapping of the lever position to rpm.
 *
 * @return The desired rpm as set by the throttle lever
 */
int readCurrentLeverThrottle();
