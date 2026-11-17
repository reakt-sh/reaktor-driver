#pragma once

#include <modes.h>

/**
 * A control command.
 */
struct ControlCommand {
    DriveMode mode;
    unsigned int target_rpm;
};
typedef struct ControlCommand tControlCommand;
