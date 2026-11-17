#pragma once

#include <control_command.h>

/**
 * Setup the safety operation procedures.
 *
 * @return true if the setup was successful
 */
bool setupSafetyControls();

/**
 * Computes the safe operation of the vehicle.
 *
 * @param currentSpeedRPM The current speed of the vehicle in RPM.
 * @param manualTargetSpeedRPM The target speed in RPM set by the manual control.
 * @param manualSwitchForward Whether the forward switch is activated.
 * @param manualSwitchReverse Whether the reverse switch is activated.
 * @param remoteControlRequested Whether remote control is requested.
 * @param remoteControlConnected Whether the remote controller is connected.
 * @param remoteControl The remote control command.
 * @param safeControl (return value) The computed safe control command.
 * @param remoteControlActivated (return value) Whether the remote control was activated.
 */
void computeSafeControl(int currentSpeedRPM, int manualTargetSpeedRPM, bool manualSwitchForward, bool manualSwitchReverse, bool remoteControlRequested, bool remoteControlConnected, tControlCommand* remoteControl, tControlCommand* safeControl, bool* remoteControlActivated);

/**
 * Triggers an emergency stop of the vehicle for internal reasons.
 */
void triggerEmergencyStop();
