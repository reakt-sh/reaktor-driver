# Auto-generated from errors.yaml
# DO NOT MODIFY!

# List of error codes and messages used by the driver
# Error codes will encoded in a one-hot bit-vector and should be continuous.

ERROR_CONTROL_MODE_SWITCH_UNSAFE = 0
ERROR_CONTROL_MODE_SWITCH_UNSAFE_MESSAGE = "Control mode switch attempted while in unsafe driving mode."
ERROR_REMOTE_CONTROL_TIMEOUT = 1
ERROR_REMOTE_CONTROL_TIMEOUT_MESSAGE = "While in active remote control mode, no control (or heartbeat) message was received in the expected interval."
ERROR_HARDWARE_NO_THROTTLE_DAC = 2
ERROR_HARDWARE_NO_THROTTLE_DAC_MESSAGE = "No connection to motor throttle control component (DAC via I2C)."
ERROR_MODES_MANUAL_MODE_SWITCH_CONFLICTING = 4
ERROR_MODES_MANUAL_MODE_SWITCH_CONFLICTING_MESSAGE = "Hardware malfuntion in manual mode switch. Both directions are active."
ERROR_MODES_VEHICLE_MOVING_WHILE_PARKING = 6
ERROR_MODES_VEHICLE_MOVING_WHILE_PARKING_MESSAGE = "Vehicle is moving while in parking mode."
ERROR_MODES_DIRECTION_CONFLICTS_MOVEMENT = 7
ERROR_MODES_DIRECTION_CONFLICTS_MOVEMENT_MESSAGE = "Requested movement direction opposes the current movement direction of the vehicle."
ERROR_MODES_DIRECTION_CONFLICTS_UNKNOWN_MOVEMENT = 8
ERROR_MODES_DIRECTION_CONFLICTS_UNKNOWN_MOVEMENT_MESSAGE = "Requested movement direction may oppose the current movement direction of the vehicle which cannot be determined."
ERROR_MODES_EMERGENCY_NOT_YET_RECOVERABLE = 9
ERROR_MODES_EMERGENCY_NOT_YET_RECOVERABLE_MESSAGE = "Emergency mode is not yet recoverable. Only requested a driving mode after the error is resolved and the vehicle did not move for the configured time."

# Mapping of error codes to human-readable messages.
ERROR_MAP = {
    0: "Connection refuesed due to protocol version mismatch between driver and remote control.",
    1: "A control message was received without a successful connection handshake beforehand.",
    2: "While in active remote control mode, no control (or heartbeat) message was received in the expected interval.",
    3: "No connection to motor throttle control component (DAC via I2C).",
    4: "Hardware malfuntion in manual mode switch. Both directions are active.",
    6: "Vehicle is moving while in parking mode.",
    7: "Requested movement direction opposes the current movement direction of the vehicle.",
    8: "Requested movement direction may oppose the current movement direction of the vehicle which cannot be determined.",
    9: "Emergency mode is not yet recoverable. Only requested a driving mode after the error is resolved and the vehicle did not move for the configured time.",
}
