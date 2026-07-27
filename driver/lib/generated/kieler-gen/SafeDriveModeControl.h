/*
 * Automatically generated C code by
 * KIELER SCCharts - The Key to Efficient Modeling
 *
 * http://rtsys.informatik.uni-kiel.de/kieler
 */
#ifndef _SAFEDRIVEMODECONTROL_H_
#define _SAFEDRIVEMODECONTROL_H_
#include "modes.h"
#include "errors.h"
#include "config_driver.h"
extern void registerError(int errorCode);


// States of regions in the SCChart
enum _SCC_R0_ManualDriveModeInference_States {
    _SCC_R0S0_SWITCH, _SCC_R0S1_NEUTRAL, _SCC_R0S2_FORWARD, _SCC_R0S3_REVERSE, _SCC_R0S4_HARDWAREMALFUNCTION
};
enum _SCC_R1_VehicleMovement_States {
    _SCC_R1S0_NOMOVEMENT, _SCC_R1S1_MOVEMENT
};
enum _SCC_R1S1R0_Direction_States {
    _SCC_R1S1R0S0_DETECT, _SCC_R1S1R0S1_UNKNOWN, _SCC_R1S1R0S2_FORWARD, _SCC_R1S1R0S3_REVERSE
};
enum _SCC_R2_RemoteControlActivation_States {
    _SCC_R2S0_MANUALCONTROL, _SCC_R2S1_REMOTECONTROL
};
enum _SCC_R3_SafeMode_States {
    _SCC_R3S0_SWITCHMODE, _SCC_R3S1_NEUTRAL, _SCC_R3S2_CHECKFORWARD, _SCC_R3S3_FORWARD, _SCC_R3S4_CHECKREVERSE, _SCC_R3S5_REVERSE, _SCC_R3S6_PARKING, _SCC_R3S7_EMERGENCYSTOP
};
enum _SCC_R3S7R0_Recovery_States {
    _SCC_R3S7R0S0_WAIT, _SCC_R3S7R0S1_READYTORECOVER
};

// Internal state of the SCChart
typedef struct {
    // Active states in regions
    enum _SCC_R0_ManualDriveModeInference_States R0_ManualDriveModeInference_activeState;
    enum _SCC_R1_VehicleMovement_States R1_VehicleMovement_activeState;
    enum _SCC_R1S1R0_Direction_States R1S1R0_Direction_activeState;
    enum _SCC_R2_RemoteControlActivation_States R2_RemoteControlActivation_activeState;
    enum _SCC_R3_SafeMode_States R3_SafeMode_activeState;
    enum _SCC_R3S7R0_Recovery_States R3S7R0_Recovery_activeState;

    // Entry handling
    char SafeDriveModeControl_entry;
    char R0S1_Neutral_entry;
    char R0S2_Forward_entry;
    char R0S3_Reverse_entry;
    char R0S4_HardwareMalfunction_entry;
    char R1S0_NoMovement_entry;
    char R1S1_Movement_entry;
    char R1S1R0S2_Forward_entry;
    char R1S1R0S3_Reverse_entry;
    char R2S0_ManualControl_entry;
    char R2S1_RemoteControl_entry;
    char R3S1_Neutral_entry;
    char R3S3_Forward_entry;
    char R3S5_Reverse_entry;
    char R3S6_Parking_entry;
    char R3S7_EmergencyStop_entry;
    char R3S7R0S0_Wait_entry;
    char R3S7R0S1_ReadyToRecover_entry;
} _scc_SafeDriveModeControl_InternalState;

typedef struct {
    // Internal state
    _scc_SafeDriveModeControl_InternalState _state;

    // Internal variables
    enum DriveMode manualDriveModeInferred;
    enum DriveMode requestedDriveMode;
    int requestedTargetSpeedRPM;
    char isMoving;
    char isMovingForward;
    char isMovingReverse;
    char _SafeDriveModeControl_local_safeToSwitch;
    int _SafeDriveModeControl_local__region0_Recovery_c;

    // IO variables
    char manualSwitchForward;
    char manualSwitchReverse;
    int manualTargetSpeedRPM;
    char manualSwitchRemoteControl;
    enum DriveMode remoteDriveMode;
    int remoteTargetSpeedRPM;
    int currentSpeedRPM;
    char issueEmergencyStop;
    char safeRemoteControl;
    enum DriveMode safeDriveMode;
    int safeTargetSpeedRPM;
    int deltaT;
} TickData;

void reset(TickData *context);
void tick(TickData *context);

#endif
