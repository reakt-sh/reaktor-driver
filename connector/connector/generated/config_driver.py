# Auto-generated from config_driver.yaml
# DO NOT MODIFY!

# Configuration parameters for the driver behavior

DRIVER_REVOLUTION_SIGNAL_BLIND_TIME = 2 # Time in millis where interrupts are ignored because the signal might be fuzzy and result in multiple signal edges.
DRIVER_BRAKING_THRESHOLD_RPM = 80 # Threshold as difference between current and target speed (in RPM) at which the brakes will be engaged to further decelerate.
DRIVER_MODES_EMERGENCY_RECOVERY_TIME_MS = 3000 # Time of no movement in ms until the driver allows leaving emergency brake mode.
DRIVER_MODES_MOVEMENT_THRESHOLD_RPM = 10 # Threshold (in RPM) at which the vehicle is considered to be moving.
