#pragma once

#include <modes.h>

/**
 * Check if it is safe to change the control mode (remote/manual).
 *
 * @return true if it is safe to change the control mode
 */
bool isControlModeChangeSafe();

/**
 * Infer the control mode from the manual control switches.
 *
 * @return The inferred control mode
 */
tDriveMode inferModeFromSwitches();

/**
 * Set the control mode that should be active next.
 *
 * @param mode The next control mode
 */
void setNextMode(tDriveMode mode);

/**
 * Trigger an emergency stop of the motor.
 */
void triggerEmergencyStop();

/**
 * Get the currently active drive mode.
 *
 * @return The current drive mode
 */
tDriveMode getCurrentMode();
