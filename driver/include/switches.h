#pragma once

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

