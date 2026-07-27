/*
 * Automatically generated C code by
 * KIELER SCCharts - The Key to Efficient Modeling
 *
 * http://rtsys.informatik.uni-kiel.de/kieler
 */
#include <stdio.h>
#include "SafeDriveModeControl.h"

/* Internal behavior */

/*
 * State ReadyToRecover (R3S7R0S1)
 * SafeDriveModeControl -> SafeMode -> EmergencyStop -> Recovery -> ReadyToRecover
 */
static inline void _scc_R3S7R0S1_ReadyToRecover_reset(TickData *context) {
    context->_state.R3S7R0S1_ReadyToRecover_entry = 1;
}
static inline void _scc_R3S7R0S1_ReadyToRecover_tick(TickData *context) {
    if (context->_state.R3S7R0S1_ReadyToRecover_entry) {
    } else {
        // During
        context->_SafeDriveModeControl_local__region0_Recovery_c += context->deltaT;
    }
    // Disable entry behavior
    context->_state.R3S7R0S1_ReadyToRecover_entry = 0;
}

/*
 * State Wait (R3S7R0S0)
 * SafeDriveModeControl -> SafeMode -> EmergencyStop -> Recovery -> Wait
 */
static inline void _scc_R3S7R0S0_Wait_reset(TickData *context) {
    context->_state.R3S7R0S0_Wait_entry = 1;
}
static inline void _scc_R3S7R0S0_Wait_tick(TickData *context) {
    if (context->_state.R3S7R0S0_Wait_entry) {
        // Entry
        context->_SafeDriveModeControl_local__region0_Recovery_c = 0;
        if (context->requestedDriveMode != EMERGENCY_STOP) {
            registerError(ERROR_MODES_EMERGENCY_NOT_YET_RECOVERABLE);
        }
    } else {
        // During
        if (context->requestedDriveMode != EMERGENCY_STOP) {
            registerError(ERROR_MODES_EMERGENCY_NOT_YET_RECOVERABLE);
        }
        context->_SafeDriveModeControl_local__region0_Recovery_c += context->deltaT;
    }
    // Disable entry behavior
    context->_state.R3S7R0S0_Wait_entry = 0;
}

/*
 * Region Recovery (R3S7R0)
 * SafeDriveModeControl -> SafeMode -> EmergencyStop -> Recovery
 */
static inline void _scc_R3S7R0_Recovery_reset(TickData *context) {
    context->_state.R3S7R0_Recovery_activeState = _SCC_R3S7R0S0_WAIT;

    // Reset states
    _scc_R3S7R0S0_Wait_reset(context);
    _scc_R3S7R0S1_ReadyToRecover_reset(context);
}
static inline void _scc_R3S7R0_Recovery_tick(TickData *context) {
    // States
    switch (context->_state.R3S7R0_Recovery_activeState) {
        case _SCC_R3S7R0S0_WAIT: // Wait
            // Execute active state
            _scc_R3S7R0S0_Wait_tick(context);
            // Check non-preemptive transitions
            if (context->isMoving || context->requestedTargetSpeedRPM > 0 || context->issueEmergencyStop) { // Priority 1
                // Go to state Wait (R3S7R0S0)
                context->_state.R3S7R0_Recovery_activeState = _SCC_R3S7R0S0_WAIT;
                _scc_R3S7R0S0_Wait_reset(context);
                _scc_R3S7R0S0_Wait_tick(context);
            }
            else if (context->_SafeDriveModeControl_local__region0_Recovery_c >= DRIVER_MODES_EMERGENCY_RECOVERY_TIME_MS) { // Priority 2
                // Go to state ReadyToRecover (R3S7R0S1)
                context->_state.R3S7R0_Recovery_activeState = _SCC_R3S7R0S1_READYTORECOVER;
                _scc_R3S7R0S1_ReadyToRecover_reset(context);
                _scc_R3S7R0S1_ReadyToRecover_tick(context);
            }
            break;
        case _SCC_R3S7R0S1_READYTORECOVER: // ReadyToRecover
            // Execute active state
            _scc_R3S7R0S1_ReadyToRecover_tick(context);
            break;
    }
}

/*
 * State EmergencyStop (R3S7)
 * SafeDriveModeControl -> SafeMode -> EmergencyStop
 */
static inline void _scc_R3S7_EmergencyStop_reset(TickData *context) {
    context->_state.R3S7_EmergencyStop_entry = 1;
    // Reset regions
    _scc_R3S7R0_Recovery_reset(context);
}
static inline void _scc_R3S7_EmergencyStop_tick(TickData *context) {
    if (context->_state.R3S7_EmergencyStop_entry) {
        // Entry
        context->_SafeDriveModeControl_local__region0_Recovery_c = 0;
        context->safeDriveMode = EMERGENCY_STOP;
        context->safeTargetSpeedRPM = 0;
        if (context->requestedDriveMode == DRIVE_FORWARD && context->isMovingReverse) {
            registerError(ERROR_MODES_DIRECTION_CONFLICTS_MOVEMENT);
        }
        if (context->requestedDriveMode == DRIVE_REVERSE && context->isMovingForward) {
            registerError(ERROR_MODES_DIRECTION_CONFLICTS_MOVEMENT);
        }
        if ((context->requestedDriveMode == DRIVE_REVERSE || context->requestedDriveMode == DRIVE_FORWARD) && context->isMoving && !context->isMovingForward && !context->isMovingReverse) {
            registerError(ERROR_MODES_DIRECTION_CONFLICTS_UNKNOWN_MOVEMENT);
        }

        // Only execute initial states upon entry to prevent immediate transitioning (except for initial connectors)
        _scc_R3S7R0S0_Wait_tick(context);

    } else {
        // Regions
        _scc_R3S7R0_Recovery_tick(context);
    }
    // Disable entry behavior
    context->_state.R3S7_EmergencyStop_entry = 0;
}

/*
 * State Parking (R3S6)
 * SafeDriveModeControl -> SafeMode -> Parking
 */
static inline void _scc_R3S6_Parking_reset(TickData *context) {
    context->_state.R3S6_Parking_entry = 1;
}
static inline void _scc_R3S6_Parking_tick(TickData *context) {
    if (context->_state.R3S6_Parking_entry) {
        // Entry
        context->safeDriveMode = PARKING;
        context->safeTargetSpeedRPM = 0;
    } else {
        // During
        if (context->isMoving) {
            registerError(ERROR_MODES_VEHICLE_MOVING_WHILE_PARKING);
        }
    }
    // Disable entry behavior
    context->_state.R3S6_Parking_entry = 0;
}

/*
 * State Reverse (R3S5)
 * SafeDriveModeControl -> SafeMode -> Reverse
 */
static inline void _scc_R3S5_Reverse_reset(TickData *context) {
    context->_state.R3S5_Reverse_entry = 1;
}
static inline void _scc_R3S5_Reverse_tick(TickData *context) {
    if (context->_state.R3S5_Reverse_entry) {
        // Entry
        context->safeDriveMode = DRIVE_REVERSE;
        context->safeTargetSpeedRPM = context->requestedTargetSpeedRPM;
    } else {
        // During
        context->safeTargetSpeedRPM = context->requestedTargetSpeedRPM;
    }
    // Disable entry behavior
    context->_state.R3S5_Reverse_entry = 0;
}

/*
 * State Forward (R3S3)
 * SafeDriveModeControl -> SafeMode -> Forward
 */
static inline void _scc_R3S3_Forward_reset(TickData *context) {
    context->_state.R3S3_Forward_entry = 1;
}
static inline void _scc_R3S3_Forward_tick(TickData *context) {
    if (context->_state.R3S3_Forward_entry) {
        // Entry
        context->safeDriveMode = DRIVE_FORWARD;
        context->safeTargetSpeedRPM = context->requestedTargetSpeedRPM;
    } else {
        // During
        context->safeTargetSpeedRPM = context->requestedTargetSpeedRPM;
    }
    // Disable entry behavior
    context->_state.R3S3_Forward_entry = 0;
}

/*
 * State Neutral (R3S1)
 * SafeDriveModeControl -> SafeMode -> Neutral
 */
static inline void _scc_R3S1_Neutral_reset(TickData *context) {
    context->_state.R3S1_Neutral_entry = 1;
}
static inline void _scc_R3S1_Neutral_tick(TickData *context) {
    if (context->_state.R3S1_Neutral_entry) {
        // Entry
        context->safeDriveMode = NEUTRAL;
        context->safeTargetSpeedRPM = 0;
    } else {
        // During
        context->safeTargetSpeedRPM = 0;
    }
    // Disable entry behavior
    context->_state.R3S1_Neutral_entry = 0;
}

/*
 * Region SafeMode (R3)
 * SafeDriveModeControl -> SafeMode
 */
static inline void _scc_R3_SafeMode_reset(TickData *context) {
    context->_state.R3_SafeMode_activeState = _SCC_R3S0_SWITCHMODE;

    // Reset states
    _scc_R3S1_Neutral_reset(context);
    _scc_R3S3_Forward_reset(context);
    _scc_R3S5_Reverse_reset(context);
    _scc_R3S6_Parking_reset(context);
    _scc_R3S7_EmergencyStop_reset(context);
}
static inline void _scc_R3_SafeMode_tick(TickData *context) {
    char handle_connector = 0;
    // States
    switch (context->_state.R3_SafeMode_activeState) {
        case _SCC_R3S1_NEUTRAL: // Neutral
            // Execute active state
            _scc_R3S1_Neutral_tick(context);
            // Check non-preemptive transitions
            if (context->requestedDriveMode != NEUTRAL) { // Priority 1
                // Go to connector SwitchMode (R3S0)
                context->_state.R3_SafeMode_activeState = _SCC_R3S0_SWITCHMODE;
                handle_connector = 1;
            }
            break;
        case _SCC_R3S3_FORWARD: // Forward
            // Execute active state
            _scc_R3S3_Forward_tick(context);
            // Check non-preemptive transitions
            if (context->requestedDriveMode != DRIVE_FORWARD) { // Priority 1
                // Go to connector SwitchMode (R3S0)
                context->_state.R3_SafeMode_activeState = _SCC_R3S0_SWITCHMODE;
                handle_connector = 1;
            }
            break;
        case _SCC_R3S5_REVERSE: // Reverse
            // Execute active state
            _scc_R3S5_Reverse_tick(context);
            // Check non-preemptive transitions
            if (context->requestedDriveMode != DRIVE_REVERSE) { // Priority 1
                // Go to connector SwitchMode (R3S0)
                context->_state.R3_SafeMode_activeState = _SCC_R3S0_SWITCHMODE;
                handle_connector = 1;
            }
            break;
        case _SCC_R3S6_PARKING: // Parking
            // Execute active state
            _scc_R3S6_Parking_tick(context);
            // Check non-preemptive transitions
            if (!context->isMoving && context->requestedDriveMode != PARKING) { // Priority 1
                // Go to connector SwitchMode (R3S0)
                context->_state.R3_SafeMode_activeState = _SCC_R3S0_SWITCHMODE;
                handle_connector = 1;
            }
            break;
        case _SCC_R3S7_EMERGENCYSTOP: // EmergencyStop
            // Execute active state
            _scc_R3S7_EmergencyStop_tick(context);
            // Check termination
            char R3S7_EmergencyStop_canTerminate = (context->_state.R3S7R0_Recovery_activeState == _SCC_R3S7R0S1_READYTORECOVER);
            // Check non-preemptive transitions
            if (R3S7_EmergencyStop_canTerminate && (context->requestedDriveMode != EMERGENCY_STOP && !context->issueEmergencyStop)) { // Priority 1
                // Go to connector SwitchMode (R3S0)
                context->_state.R3_SafeMode_activeState = _SCC_R3S0_SWITCHMODE;
                handle_connector = 1;
            }
            break;
        case _SCC_R3S0_SWITCHMODE:
        case _SCC_R3S2_CHECKFORWARD:
        case _SCC_R3S4_CHECKREVERSE:
            // Intentional fallthrough
            // Connectors are transient and handled separately to enable instantaneous traversal
            handle_connector = 1;
            break;
    }
    // Instantaneous connector traversal
    if (handle_connector) {
        if (context->_state.R3_SafeMode_activeState == _SCC_R3S0_SWITCHMODE) {  // SwitchMode
            if (context->requestedDriveMode == EMERGENCY_STOP || context->issueEmergencyStop) { // Priority 1
                // Go to state EmergencyStop (R3S7)
                context->_state.R3_SafeMode_activeState = _SCC_R3S7_EMERGENCYSTOP;
                _scc_R3S7_EmergencyStop_reset(context);
                _scc_R3S7_EmergencyStop_tick(context);
            }
            else if (context->requestedDriveMode == NEUTRAL) { // Priority 2
                // Go to state Neutral (R3S1)
                context->_state.R3_SafeMode_activeState = _SCC_R3S1_NEUTRAL;
                _scc_R3S1_Neutral_reset(context);
                _scc_R3S1_Neutral_tick(context);
            }
            else if (context->requestedDriveMode == DRIVE_FORWARD) { // Priority 3
                // Go to next connector CheckForward (R3S2)
                context->_state.R3_SafeMode_activeState = _SCC_R3S2_CHECKFORWARD;
            }
            else if (context->requestedDriveMode == DRIVE_REVERSE) { // Priority 4
                // Go to next connector CheckReverse (R3S4)
                context->_state.R3_SafeMode_activeState = _SCC_R3S4_CHECKREVERSE;
            }
            else if (context->requestedDriveMode == PARKING) { // Priority 5
                // Go to state Parking (R3S6)
                context->_state.R3_SafeMode_activeState = _SCC_R3S6_PARKING;
                _scc_R3S6_Parking_reset(context);
                _scc_R3S6_Parking_tick(context);
            }
            else if (1) { // Priority 6
                // Go to state EmergencyStop (R3S7)
                context->_state.R3_SafeMode_activeState = _SCC_R3S7_EMERGENCYSTOP;
                _scc_R3S7_EmergencyStop_reset(context);
                _scc_R3S7_EmergencyStop_tick(context);
            }
        }
        if (context->_state.R3_SafeMode_activeState == _SCC_R3S2_CHECKFORWARD) {  // CheckForward
            if (!context->isMoving || context->isMovingForward) { // Priority 1
                // Go to state Forward (R3S3)
                context->_state.R3_SafeMode_activeState = _SCC_R3S3_FORWARD;
                _scc_R3S3_Forward_reset(context);
                _scc_R3S3_Forward_tick(context);
            }
            else if (context->isMoving && !context->isMovingReverse && !context->safeRemoteControl) { // Priority 2
                // Go to state Forward (R3S3)
                context->_state.R3_SafeMode_activeState = _SCC_R3S3_FORWARD;
                _scc_R3S3_Forward_reset(context);
                _scc_R3S3_Forward_tick(context);
            }
            else if (1) { // Priority 3
                // Go to state EmergencyStop (R3S7)
                context->_state.R3_SafeMode_activeState = _SCC_R3S7_EMERGENCYSTOP;
                _scc_R3S7_EmergencyStop_reset(context);
                _scc_R3S7_EmergencyStop_tick(context);
            }
        }
        if (context->_state.R3_SafeMode_activeState == _SCC_R3S4_CHECKREVERSE) {  // CheckReverse
            if (!context->isMoving || context->isMovingReverse) { // Priority 1
                // Go to state Reverse (R3S5)
                context->_state.R3_SafeMode_activeState = _SCC_R3S5_REVERSE;
                _scc_R3S5_Reverse_reset(context);
                _scc_R3S5_Reverse_tick(context);
            }
            else if (context->isMoving && !context->isMovingForward && !context->safeRemoteControl) { // Priority 2
                // Go to state Reverse (R3S5)
                context->_state.R3_SafeMode_activeState = _SCC_R3S5_REVERSE;
                _scc_R3S5_Reverse_reset(context);
                _scc_R3S5_Reverse_tick(context);
            }
            else if (1) { // Priority 3
                // Go to state EmergencyStop (R3S7)
                context->_state.R3_SafeMode_activeState = _SCC_R3S7_EMERGENCYSTOP;
                _scc_R3S7_EmergencyStop_reset(context);
                _scc_R3S7_EmergencyStop_tick(context);
            }
        }
    }
}

/*
 * State RemoteControl (R2S1)
 * SafeDriveModeControl -> RemoteControlActivation -> RemoteControl
 */
static inline void _scc_R2S1_RemoteControl_reset(TickData *context) {
    context->_state.R2S1_RemoteControl_entry = 1;
}
static inline void _scc_R2S1_RemoteControl_tick(TickData *context) {
    if (context->_state.R2S1_RemoteControl_entry) {
        // Entry
        context->safeRemoteControl = 1;
        context->requestedDriveMode = context->remoteDriveMode;
        context->requestedTargetSpeedRPM = context->remoteTargetSpeedRPM;
    } else {
        // During
        context->requestedDriveMode = context->remoteDriveMode;
        context->requestedTargetSpeedRPM = context->remoteTargetSpeedRPM;
    }
    // Disable entry behavior
    context->_state.R2S1_RemoteControl_entry = 0;
}

/*
 * State ManualControl (R2S0)
 * SafeDriveModeControl -> RemoteControlActivation -> ManualControl
 */
static inline void _scc_R2S0_ManualControl_reset(TickData *context) {
    context->_state.R2S0_ManualControl_entry = 1;
}
static inline void _scc_R2S0_ManualControl_tick(TickData *context) {
    if (context->_state.R2S0_ManualControl_entry) {
        // Entry
        context->_SafeDriveModeControl_local_safeToSwitch = 0;
        context->safeRemoteControl = 0;
        context->requestedDriveMode = context->manualDriveModeInferred;
        context->requestedTargetSpeedRPM = context->manualTargetSpeedRPM;
    } else {
        // During
        context->requestedDriveMode = context->manualDriveModeInferred;
        context->requestedTargetSpeedRPM = context->manualTargetSpeedRPM;
        context->_SafeDriveModeControl_local_safeToSwitch = !context->isMoving && context->manualTargetSpeedRPM == 0 && context->remoteTargetSpeedRPM == 0 && (context->manualDriveModeInferred == NEUTRAL || context->manualDriveModeInferred == PARKING) && (context->safeDriveMode == NEUTRAL || context->safeDriveMode == PARKING) && (context->remoteDriveMode == NEUTRAL || context->remoteDriveMode == PARKING);
    }
    // Disable entry behavior
    context->_state.R2S0_ManualControl_entry = 0;
}

/*
 * Region RemoteControlActivation (R2)
 * SafeDriveModeControl -> RemoteControlActivation
 */
static inline void _scc_R2_RemoteControlActivation_reset(TickData *context) {
    context->_state.R2_RemoteControlActivation_activeState = _SCC_R2S0_MANUALCONTROL;

    // Reset states
    _scc_R2S0_ManualControl_reset(context);
    _scc_R2S1_RemoteControl_reset(context);
}
static inline void _scc_R2_RemoteControlActivation_tick(TickData *context) {
    // States
    switch (context->_state.R2_RemoteControlActivation_activeState) {
        case _SCC_R2S0_MANUALCONTROL: // ManualControl
            // Execute active state
            _scc_R2S0_ManualControl_tick(context);
            // Check non-preemptive transitions
            if (context->manualSwitchRemoteControl && context->_SafeDriveModeControl_local_safeToSwitch) { // Priority 1
                // Go to state RemoteControl (R2S1)
                context->_state.R2_RemoteControlActivation_activeState = _SCC_R2S1_REMOTECONTROL;
                _scc_R2S1_RemoteControl_reset(context);
                _scc_R2S1_RemoteControl_tick(context);
            }
            break;
        case _SCC_R2S1_REMOTECONTROL: // RemoteControl
            // Execute active state
            _scc_R2S1_RemoteControl_tick(context);
            // Check non-preemptive transitions
            if (!context->manualSwitchRemoteControl) { // Priority 1
                // Go to state ManualControl (R2S0)
                context->_state.R2_RemoteControlActivation_activeState = _SCC_R2S0_MANUALCONTROL;
                _scc_R2S0_ManualControl_reset(context);
                _scc_R2S0_ManualControl_tick(context);
            }
            break;
    }
}

/*
 * State Reverse (R1S1R0S3)
 * SafeDriveModeControl -> VehicleMovement -> Movement -> Direction -> Reverse
 */
static inline void _scc_R1S1R0S3_Reverse_reset(TickData *context) {
    context->_state.R1S1R0S3_Reverse_entry = 1;
}
static inline void _scc_R1S1R0S3_Reverse_tick(TickData *context) {
    if (context->_state.R1S1R0S3_Reverse_entry) {
        // Entry
        context->isMovingReverse = 1;
    } else {
    }
    // Disable entry behavior
    context->_state.R1S1R0S3_Reverse_entry = 0;
}

/*
 * State Forward (R1S1R0S2)
 * SafeDriveModeControl -> VehicleMovement -> Movement -> Direction -> Forward
 */
static inline void _scc_R1S1R0S2_Forward_reset(TickData *context) {
    context->_state.R1S1R0S2_Forward_entry = 1;
}
static inline void _scc_R1S1R0S2_Forward_tick(TickData *context) {
    if (context->_state.R1S1R0S2_Forward_entry) {
        // Entry
        context->isMovingForward = 1;
    } else {
    }
    // Disable entry behavior
    context->_state.R1S1R0S2_Forward_entry = 0;
}

/*
 * Region Direction (R1S1R0)
 * SafeDriveModeControl -> VehicleMovement -> Movement -> Direction
 */
static inline void _scc_R1S1R0_Direction_reset(TickData *context) {
    context->_state.R1S1R0_Direction_activeState = _SCC_R1S1R0S0_DETECT;

    // Reset states
    _scc_R1S1R0S2_Forward_reset(context);
    _scc_R1S1R0S3_Reverse_reset(context);
}
static inline void _scc_R1S1R0_Direction_tick(TickData *context) {
    char handle_connector = 0;
    // States
    switch (context->_state.R1S1R0_Direction_activeState) {
        case _SCC_R1S1R0S1_UNKNOWN: // Unknown
            // Check non-preemptive transitions
            if (1) { // Priority 1
                // Go to connector Detect (R1S1R0S0)
                context->_state.R1S1R0_Direction_activeState = _SCC_R1S1R0S0_DETECT;
                handle_connector = 1;
            }
            break;
        case _SCC_R1S1R0S2_FORWARD: // Forward
            // Execute active state
            _scc_R1S1R0S2_Forward_tick(context);
            break;
        case _SCC_R1S1R0S3_REVERSE: // Reverse
            // Execute active state
            _scc_R1S1R0S3_Reverse_tick(context);
            break;
        case _SCC_R1S1R0S0_DETECT:
            // Intentional fallthrough
            // Connectors are transient and handled separately to enable instantaneous traversal
            handle_connector = 1;
            break;
    }
    // Instantaneous connector traversal
    if (handle_connector) {
        if (context->_state.R1S1R0_Direction_activeState == _SCC_R1S1R0S0_DETECT) {  // Detect
            if (context->safeTargetSpeedRPM > 0 && context->safeDriveMode == DRIVE_FORWARD) { // Priority 1
                // Go to state Forward (R1S1R0S2)
                context->_state.R1S1R0_Direction_activeState = _SCC_R1S1R0S2_FORWARD;
                _scc_R1S1R0S2_Forward_reset(context);
                _scc_R1S1R0S2_Forward_tick(context);
            }
            else if (context->safeTargetSpeedRPM > 0 && context->safeDriveMode == DRIVE_REVERSE) { // Priority 2
                // Go to state Reverse (R1S1R0S3)
                context->_state.R1S1R0_Direction_activeState = _SCC_R1S1R0S3_REVERSE;
                _scc_R1S1R0S3_Reverse_reset(context);
                _scc_R1S1R0S3_Reverse_tick(context);
            }
            else if (1) { // Priority 3
                // Go to state Unknown (R1S1R0S1)
                context->_state.R1S1R0_Direction_activeState = _SCC_R1S1R0S1_UNKNOWN;
            }
        }
    }
}

/*
 * State Movement (R1S1)
 * SafeDriveModeControl -> VehicleMovement -> Movement
 */
static inline void _scc_R1S1_Movement_reset(TickData *context) {
    context->_state.R1S1_Movement_entry = 1;
    // Reset regions
    _scc_R1S1R0_Direction_reset(context);
}
static inline void _scc_R1S1_Movement_tick(TickData *context) {
    if (context->_state.R1S1_Movement_entry) {
        // Entry
        context->isMoving = 1;

        // Only execute initial states upon entry to prevent immediate transitioning (except for initial connectors)
        _scc_R1S1R0_Direction_tick(context);

    } else {
        // Regions
        _scc_R1S1R0_Direction_tick(context);
    }
    // Disable entry behavior
    context->_state.R1S1_Movement_entry = 0;
}

/*
 * State NoMovement (R1S0)
 * SafeDriveModeControl -> VehicleMovement -> NoMovement
 */
static inline void _scc_R1S0_NoMovement_reset(TickData *context) {
    context->_state.R1S0_NoMovement_entry = 1;
}
static inline void _scc_R1S0_NoMovement_tick(TickData *context) {
    if (context->_state.R1S0_NoMovement_entry) {
        // Entry
        context->isMoving = 0;
        context->isMovingForward = 0;
        context->isMovingReverse = 0;
    } else {
    }
    // Disable entry behavior
    context->_state.R1S0_NoMovement_entry = 0;
}

/*
 * Region VehicleMovement (R1)
 * SafeDriveModeControl -> VehicleMovement
 */
static inline void _scc_R1_VehicleMovement_reset(TickData *context) {
    context->_state.R1_VehicleMovement_activeState = _SCC_R1S0_NOMOVEMENT;

    // Reset states
    _scc_R1S0_NoMovement_reset(context);
    _scc_R1S1_Movement_reset(context);
}
static inline void _scc_R1_VehicleMovement_tick(TickData *context) {
    // States
    switch (context->_state.R1_VehicleMovement_activeState) {
        case _SCC_R1S0_NOMOVEMENT: // NoMovement
            // Execute active state
            _scc_R1S0_NoMovement_tick(context);
            // Check non-preemptive transitions
            if (context->currentSpeedRPM > DRIVER_MODES_MOVEMENT_THRESHOLD_RPM) { // Priority 1
                // Go to state Movement (R1S1)
                context->_state.R1_VehicleMovement_activeState = _SCC_R1S1_MOVEMENT;
                _scc_R1S1_Movement_reset(context);
                _scc_R1S1_Movement_tick(context);
            }
            break;
        case _SCC_R1S1_MOVEMENT: // Movement
            // Check preemptive transitions (strong aborts)
            if (context->currentSpeedRPM < DRIVER_MODES_MOVEMENT_THRESHOLD_RPM) { // Priority 1
                // Go to state NoMovement (R1S0)
                context->_state.R1_VehicleMovement_activeState = _SCC_R1S0_NOMOVEMENT;
                _scc_R1S0_NoMovement_reset(context);
                _scc_R1S0_NoMovement_tick(context);
                break;
            }
            // Execute active state
            _scc_R1S1_Movement_tick(context);
            break;
    }
}

/*
 * State HardwareMalfunction (R0S4)
 * SafeDriveModeControl -> ManualDriveModeInference -> HardwareMalfunction
 */
static inline void _scc_R0S4_HardwareMalfunction_reset(TickData *context) {
    context->_state.R0S4_HardwareMalfunction_entry = 1;
}
static inline void _scc_R0S4_HardwareMalfunction_tick(TickData *context) {
    if (context->_state.R0S4_HardwareMalfunction_entry) {
        // Entry
        context->manualDriveModeInferred = EMERGENCY_STOP;
        registerError(ERROR_MODES_MANUAL_MODE_SWITCH_CONFLICTING);
    } else {
        // During
        registerError(ERROR_MODES_MANUAL_MODE_SWITCH_CONFLICTING);
    }
    // Disable entry behavior
    context->_state.R0S4_HardwareMalfunction_entry = 0;
}

/*
 * State Reverse (R0S3)
 * SafeDriveModeControl -> ManualDriveModeInference -> Reverse
 */
static inline void _scc_R0S3_Reverse_reset(TickData *context) {
    context->_state.R0S3_Reverse_entry = 1;
}
static inline void _scc_R0S3_Reverse_tick(TickData *context) {
    if (context->_state.R0S3_Reverse_entry) {
        // Entry
        context->manualDriveModeInferred = DRIVE_REVERSE;
    } else {
    }
    // Disable entry behavior
    context->_state.R0S3_Reverse_entry = 0;
}

/*
 * State Forward (R0S2)
 * SafeDriveModeControl -> ManualDriveModeInference -> Forward
 */
static inline void _scc_R0S2_Forward_reset(TickData *context) {
    context->_state.R0S2_Forward_entry = 1;
}
static inline void _scc_R0S2_Forward_tick(TickData *context) {
    if (context->_state.R0S2_Forward_entry) {
        // Entry
        context->manualDriveModeInferred = DRIVE_FORWARD;
    } else {
    }
    // Disable entry behavior
    context->_state.R0S2_Forward_entry = 0;
}

/*
 * State Neutral (R0S1)
 * SafeDriveModeControl -> ManualDriveModeInference -> Neutral
 */
static inline void _scc_R0S1_Neutral_reset(TickData *context) {
    context->_state.R0S1_Neutral_entry = 1;
}
static inline void _scc_R0S1_Neutral_tick(TickData *context) {
    if (context->_state.R0S1_Neutral_entry) {
        // Entry
        context->manualDriveModeInferred = NEUTRAL;
    } else {
    }
    // Disable entry behavior
    context->_state.R0S1_Neutral_entry = 0;
}

/*
 * Region ManualDriveModeInference (R0)
 * SafeDriveModeControl -> ManualDriveModeInference
 */
static inline void _scc_R0_ManualDriveModeInference_reset(TickData *context) {
    context->_state.R0_ManualDriveModeInference_activeState = _SCC_R0S0_SWITCH;

    // Reset states
    _scc_R0S1_Neutral_reset(context);
    _scc_R0S2_Forward_reset(context);
    _scc_R0S3_Reverse_reset(context);
    _scc_R0S4_HardwareMalfunction_reset(context);
}
static inline void _scc_R0_ManualDriveModeInference_tick(TickData *context) {
    char handle_connector = 0;
    // States
    switch (context->_state.R0_ManualDriveModeInference_activeState) {
        case _SCC_R0S1_NEUTRAL: // Neutral
            // Execute active state
            _scc_R0S1_Neutral_tick(context);
            // Check non-preemptive transitions
            if (context->manualSwitchForward || context->manualSwitchReverse) { // Priority 1
                // Go to connector Switch (R0S0)
                context->_state.R0_ManualDriveModeInference_activeState = _SCC_R0S0_SWITCH;
                handle_connector = 1;
            }
            break;
        case _SCC_R0S2_FORWARD: // Forward
            // Execute active state
            _scc_R0S2_Forward_tick(context);
            // Check non-preemptive transitions
            if (!context->manualSwitchForward || context->manualSwitchReverse) { // Priority 1
                // Go to connector Switch (R0S0)
                context->_state.R0_ManualDriveModeInference_activeState = _SCC_R0S0_SWITCH;
                handle_connector = 1;
            }
            break;
        case _SCC_R0S3_REVERSE: // Reverse
            // Execute active state
            _scc_R0S3_Reverse_tick(context);
            // Check non-preemptive transitions
            if (context->manualSwitchForward || !context->manualSwitchReverse) { // Priority 1
                // Go to connector Switch (R0S0)
                context->_state.R0_ManualDriveModeInference_activeState = _SCC_R0S0_SWITCH;
                handle_connector = 1;
            }
            break;
        case _SCC_R0S4_HARDWAREMALFUNCTION: // HardwareMalfunction
            // Check preemptive transitions (strong aborts)
            if (!context->manualSwitchForward || !context->manualSwitchReverse) { // Priority 1
                // Go to connector Switch (R0S0)
                context->_state.R0_ManualDriveModeInference_activeState = _SCC_R0S0_SWITCH;
                handle_connector = 1;
                break;
            }
            // Execute active state
            _scc_R0S4_HardwareMalfunction_tick(context);
            break;
        case _SCC_R0S0_SWITCH:
            // Intentional fallthrough
            // Connectors are transient and handled separately to enable instantaneous traversal
            handle_connector = 1;
            break;
    }
    // Instantaneous connector traversal
    if (handle_connector) {
        if (context->_state.R0_ManualDriveModeInference_activeState == _SCC_R0S0_SWITCH) {  // Switch
            if (context->manualSwitchForward && context->manualSwitchReverse) { // Priority 1
                // Go to state HardwareMalfunction (R0S4)
                context->_state.R0_ManualDriveModeInference_activeState = _SCC_R0S4_HARDWAREMALFUNCTION;
                _scc_R0S4_HardwareMalfunction_reset(context);
                _scc_R0S4_HardwareMalfunction_tick(context);
            }
            else if (context->manualSwitchForward) { // Priority 2
                // Go to state Forward (R0S2)
                context->_state.R0_ManualDriveModeInference_activeState = _SCC_R0S2_FORWARD;
                _scc_R0S2_Forward_reset(context);
                _scc_R0S2_Forward_tick(context);
            }
            else if (context->manualSwitchReverse) { // Priority 3
                // Go to state Reverse (R0S3)
                context->_state.R0_ManualDriveModeInference_activeState = _SCC_R0S3_REVERSE;
                _scc_R0S3_Reverse_reset(context);
                _scc_R0S3_Reverse_tick(context);
            }
            else if (1) { // Priority 4
                // Go to state Neutral (R0S1)
                context->_state.R0_ManualDriveModeInference_activeState = _SCC_R0S1_NEUTRAL;
                _scc_R0S1_Neutral_reset(context);
                _scc_R0S1_Neutral_tick(context);
            }
        }
    }
}

/*
 * State SafeDriveModeControl
 */
static inline void _scc_SafeDriveModeControl_reset(TickData *context) {
    context->_state.SafeDriveModeControl_entry = 1;
    // Reset regions
    _scc_R0_ManualDriveModeInference_reset(context);
    _scc_R1_VehicleMovement_reset(context);
    _scc_R2_RemoteControlActivation_reset(context);
    _scc_R3_SafeMode_reset(context);
}
static inline void _scc_SafeDriveModeControl_tick(TickData *context) {
    if (context->_state.SafeDriveModeControl_entry) {
        // Entry
        context->safeRemoteControl = 0;
        context->safeDriveMode = NEUTRAL;
        context->safeTargetSpeedRPM = 0;
        context->manualDriveModeInferred = NEUTRAL;
        context->requestedDriveMode = NEUTRAL;
        context->requestedTargetSpeedRPM = 0;
        context->isMoving = 0;
        context->isMovingForward = 0;
        context->isMovingReverse = 0;
        context->deltaT = 0;

        // Only execute initial states upon entry to prevent immediate transitioning (except for initial connectors)
        _scc_R0_ManualDriveModeInference_tick(context);
        _scc_R1S0_NoMovement_tick(context);
        _scc_R2S0_ManualControl_tick(context);
        _scc_R3_SafeMode_tick(context);

    } else {
        // Regions
        _scc_R0_ManualDriveModeInference_tick(context);
        _scc_R1_VehicleMovement_tick(context);
        _scc_R2_RemoteControlActivation_tick(context);
        _scc_R3_SafeMode_tick(context);
    }
    // Disable entry behavior
    context->_state.SafeDriveModeControl_entry = 0;
}

/* External interface */
void reset(TickData *context) {
    _scc_SafeDriveModeControl_reset(context);
}

void tick(TickData *context) {
    _scc_SafeDriveModeControl_tick(context);
}
