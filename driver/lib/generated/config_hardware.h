// Auto-generated from config_hardware.yaml
// DO NOT MODIFY!

// Configuration parameters for the driver board and connected hardware

#pragma once
#include <Arduino.h>


// Driver board pin specifications
const int BOARD_PIN_IN_MOTOR_METER = 2; // Pin for reading motor speed.
const int BOARD_PIN_IN_MANUAL_THROTTLE = A0; // Pin for reading throttle lever position.
const int BOARD_PIN_IN_DAC = A1; // Pin for reading output of DAC.
const int BOARD_PIN_IN_ONBOARD_ENABLE_REMOTE_CONTROL = 7; // Pin for reading onboard switch for enabling remote control.
const int BOARD_PIN_IN_MANUAL_FORWARD = 51; // Pin for reading manual forward switch.
const int BOARD_PIN_IN_MANUAL_REVERSE = 47; // Pin for reading manual reverse switch.
const int BOARD_PIN_OUT_MOTOR_FORWARD = 6; // Pin for engaging forward drive of the motor.
const int BOARD_PIN_OUT_MOTOR_REVERSE = 5; // Pin for engaging reverse drive in the motor.
const int BOARD_PIN_OUT_MOTOR_BRAKE = 4; // Pin for engaging brake of the motor.
const int BOARD_PIN_OUT_MOTOR_REGENERATIVE = 3; // Pin for engaging regenerative braking of the motor.

// Analog conversion specifications
const float V_REF = 5.0; // Analog reference voltage.
const uint32_t DAC_STEPS = (1 << int(12)) - 1; // Number of steps in MCP 4725 (12 bit).
const uint32_t ADC_STEPS = (1 << int(10)) - 1; // Number of steps in Arduino ADC (10 bit).

// Motor specifications
const unsigned int MOTOR_MAX_RPM = 1125; // Maximum motor speed in RPM.
const unsigned int MOTOR_MIN_RPM = 45; // Motor will not move at speed lower than this. This will also influences the waiting time until the motor is considered stopped.
const int MOTOR_SIGNAL_EDGES_PER_REVOLUTION = 3; // Number of signal edges (rising edges) per motor revolution. E.g. for a hall sensor with 3 magnets this is 3. Note that only every third edge is a full revolution.

// Throttle lever specifications
const double THROTTLE_LEVER_LOWER_DEADZONE = 0.01; // Lower deadzone for throttle lever (0.0 - 1.0).
const double THROTTLE_LEVER_UPPER_DEADZONE = 0.09; // Upper deadzone for throttle lever (0.0 - 1.0).
