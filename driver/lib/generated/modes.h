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