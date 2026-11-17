#pragma once

/**
 * Setup the interface elements on the panel (LEDs, display, knob).
 *
 * @return true if the setup was successful
 */
bool setupPanel();

/**
 * Update the information on panel interface elements.
 *
 * @param currentSpeedRPM The current speed of the vehicle in RPM.
 * @param safeControl The current safe control command.
 * @param remoteControlActive Whether the remote control is currently active.
 */
void updatePanel(int currentSpeedRPM, tControlCommand* safeControl, bool remoteControlActive);
