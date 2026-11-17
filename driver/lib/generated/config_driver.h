// Auto-generated from config_driver.yaml
// DO NOT MODIFY!

// Configuration parameters for the driver behavior

#pragma once
#include <Arduino.h>

const int DRIVER_REVOLUTION_SIGNAL_BLIND_TIME = 2; // Time in millis where interrupts are ignored because the signal might be fuzzy and result in multiple signal edges.
const int DRIVER_BRAKING_DECELERATION_OFFSET_RPM = 15; // Offset in RPM for the deceleration braking behavior. If the negative difference between current and target speed (as absolute value) exceeds this offset, braking behavior will start.
const int DRIVER_BRAKING_OVERSHOOT_LIMIT_RPM = 361; // Limit in RPM for the overshoot braking behavior. As soon as the actual speed is above this value over the target speed, braking behavior will start.
const int DRIVER_PANEL_DISPLAY_SPEED_REFRESH_DELAY_MS = 100; // Delay in millis between speed value updates on the display. When the mode changes the value will be updated immediately.
const int DRIVER_THROTTLE_LEVER_MOVING_AVERAGE_WINDOW_SIZE = 30; // Window size for the moving average calculation of the throttle lever. This affects the smoothing of the throttle input in manual control mode.
const int DRIVER_THROTTLE_LEVER_GRANULARITY = 2; // Granularity of the throttle lever rpm value. Changes below this value will be ignored. This only affects the manual control mode.
const int DRIVER_MODES_EMERGENCY_RECOVERY_TIME_MS = 3000; // Time of no movement in ms until the driver allows leaving emergency brake mode.
const int DRIVER_MODES_MOVEMENT_THRESHOLD_RPM = 10; // Threshold (in RPM) at which the vehicle is considered to be moving.
