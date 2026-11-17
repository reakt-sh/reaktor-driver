#pragma once

#include <control_command.h>

/**
 * Setup the speed control handler.
 *
 * @return true if the setup was successful
 */
bool setupSpeedHandler();

/**
 * Control motor speed.
 *
 * @param currentRPM The current RPM of the motor.
 * @param safeControl The computed safe control command to apply.
 */
void controlSpeed(int currentRPM, tControlCommand* safeControl);

