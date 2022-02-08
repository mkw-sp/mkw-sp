#pragma once

#include <game/ui/Layout.h>
#include "CtrlRaceBase.h"

typedef enum {
    // The order here must match the order in the `trick` field of RaceInputState
    kDpadState_Off,
    kDpadState_Up,
    kDpadState_Down,
    kDpadState_Left,
    kDpadState_Right,
    kDpadState_Count,
} DpadState;

typedef enum {
    kAccelState_Off,
    kAccelState_Pressed,
    kAccelState_Count,
} AccelState;

// On modern controllers, triggers are analog; on the Wii it's binary
typedef enum {
    kTriggerState_Off,
    kTriggerState_Pressed,
    kTriggerState_Count,
} TriggerState;

typedef enum {
    kTrigger_L,
    kTrigger_R,
    kTrigger_BrakeDrift,  // 200cc only
    kTrigger_Count,
} Triggers;

typedef struct {
    CtrlRaceBase;
    lyt_Pane *dpadPanes[kDpadState_Count];
    lyt_Pane *accelPanes[kAccelState_Count];
    lyt_Pane *triggerPanes[kTrigger_Count][kAccelState_Count];
    lyt_Pane *cstickPane;
    Vec3 cstickOrigin;

    DpadState dpadState;
    s8 dpadTimer;  // Hold the DPAD press for DPAD_HOLD_FOR_N_FRAMES frames
    AccelState accelState;
    TriggerState triggerStates[kTrigger_Count];
    Vec2 cstickState;

    u32 elocalPlayerId;
} CtrlRaceInputDisplay;

enum {
    DPAD_HOLD_FOR_N_FRAMES = 10,
};

CtrlRaceInputDisplay *CtrlRaceInputDisplay_ct(CtrlRaceInputDisplay *this);

void CtrlRaceInputDisplay_load(
        CtrlRaceInputDisplay *this, u32 localPlayerCount, u32 localPlayerId);
