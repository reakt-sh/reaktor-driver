#pragma once

#include "mode.h"

/**
 * Setup the detection of manual switches and control for command switches.
 *
 * @return true if the setup was successful
 */
bool setupSwitches();

/**
 * @return true if any switch for enabling remote control is engaged
 */
bool isRemoteControlSwitchEngaged();

/**
 * @return true if manual switch for enabling forward direction is engaged
 */
bool isManualForwardSwitchEngaged();

/**
 * @return true if manual switch for enabling reverse direction is engaged
 */
bool isManualReverseSwitchEngaged();

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
