#pragma once

// Just forward mode definitions in communication protocol
#include <communication_bp.h>

/**
 * The drive modes.
 */
enum DriveMode {
  NEUTRAL = DRIVE_MODE_NEUTRAL,
  DRIVE_FORWARD = DRIVE_MODE_FORWARD,
  DRIVE_REVERSE = DRIVE_MODE_REVERSE, // Cannot be called REVERSE due to clash with define in PID library :(
  PARKING = DRIVE_MODE_PARKING,
  EMERGENCY_STOP = DRIVE_MODE_EMERGENCY_STOP
};
typedef enum DriveMode tDriveMode;

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
