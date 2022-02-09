#include "CtrlRaceInputDisplay.h"
#include <game/kart/KartObjectManager.h>
#include <game/system/RaceManager.h>
#include <game/system/SaveManager.h>
#include <revolution.h>
#include <stdio.h>

static const CtrlRaceBase_vt s_CtrlRaceInputDisplay_vt;

// BRLYT pane data
#define ACCEL_ON_PANE "ACCEL_ON"
#define ACCEL_OFF_PANE "ACCEL_OFF"

#define DPAD_OFF_PANE "DPAD_OFF"
#define DPAD_RIGHT_PANE "DPAD_RIGHT"
#define DPAD_UP_PANE "DPAD_UP"
#define DPAD_LEFT_PANE "DPAD_LEFT"
#define DPAD_DOWN_PANE "DPAD_DOWN"

#define CSTICK_PANE "CSTICK"

#define TRIG_L_ON_PANE "TRIG_L_ON"
#define TRIG_L_OFF_PANE "TRIG_L_OFF"

#define TRIG_R_ON_PANE "TRIG_R_ON"
#define TRIG_R_OFF_PANE "TRIG_R_OFF"

#define TRIG_BRAKEDRIFT_ON_PANE "TRIG_BD_ON"
#define TRIG_BRAKEDRIFT_OFF_PANE "TRIG_BD_OFF"

CtrlRaceInputDisplay *CtrlRaceInputDisplay_ct(CtrlRaceInputDisplay *this) {
    LayoutUIControl_ct(this);
    this->vt = &s_CtrlRaceInputDisplay_vt;

    this->dpadState = kDpadState_Off;
    this->dpadTimer = 0;
    this->accelState = kAccelState_Off;
    this->triggerStates[kTrigger_L] = kTriggerState_Off;
    this->triggerStates[kTrigger_R] = kTriggerState_Off;
    this->triggerStates[kTrigger_BrakeDrift] = kTriggerState_Off;
    this->cstickState.x = 0.0f;
    this->cstickState.y = 0.0f;

    return this;
}

static void CtrlRaceInputDisplay_dt(UIControl *base, s32 type) {
    CtrlRaceInputDisplay *this = (CtrlRaceInputDisplay *)base;

    CtrlRaceBase_dt(this, 0);
    if (type > 0) {
        delete (this);
    }
}

static void CtrlRaceInputDisplay_initSelf(UIControl *base) {
    CtrlRaceInputDisplay *this = (CtrlRaceInputDisplay *)base;

    CtrlRaceBase_initSelf(this);

    // Reset
    this->dpadPanes[kDpadState_Off] =
            Layout_findPaneByName(&this->mainLayout, DPAD_OFF_PANE);
    this->dpadPanes[kDpadState_Right] =
            Layout_findPaneByName(&this->mainLayout, DPAD_RIGHT_PANE);
    this->dpadPanes[kDpadState_Up] =
            Layout_findPaneByName(&this->mainLayout, DPAD_UP_PANE);
    this->dpadPanes[kDpadState_Left] =
            Layout_findPaneByName(&this->mainLayout, DPAD_LEFT_PANE);
    this->dpadPanes[kDpadState_Down] =
            Layout_findPaneByName(&this->mainLayout, DPAD_DOWN_PANE);

    this->accelPanes[kAccelState_Off] =
            Layout_findPaneByName(&this->mainLayout, ACCEL_OFF_PANE);
    this->accelPanes[kAccelState_Pressed] =
            Layout_findPaneByName(&this->mainLayout, ACCEL_ON_PANE);

    this->accelPanes[kAccelState_Off] =
            Layout_findPaneByName(&this->mainLayout, ACCEL_OFF_PANE);
    this->accelPanes[kAccelState_Pressed] =
            Layout_findPaneByName(&this->mainLayout, ACCEL_ON_PANE);

    this->triggerPanes[kTrigger_L][kTriggerState_Off] =
            Layout_findPaneByName(&this->mainLayout, TRIG_L_OFF_PANE);
    this->triggerPanes[kTrigger_L][kTriggerState_Pressed] =
            Layout_findPaneByName(&this->mainLayout, TRIG_L_ON_PANE);

    this->triggerPanes[kTrigger_R][kTriggerState_Off] =
            Layout_findPaneByName(&this->mainLayout, TRIG_R_OFF_PANE);
    this->triggerPanes[kTrigger_R][kTriggerState_Pressed] =
            Layout_findPaneByName(&this->mainLayout, TRIG_R_ON_PANE);

    this->triggerPanes[kTrigger_BrakeDrift][kTriggerState_Off] =
            Layout_findPaneByName(&this->mainLayout, TRIG_BRAKEDRIFT_OFF_PANE);
    this->triggerPanes[kTrigger_BrakeDrift][kTriggerState_Pressed] =
            Layout_findPaneByName(&this->mainLayout, TRIG_BRAKEDRIFT_ON_PANE);

    this->cstickPane = Layout_findPaneByName(&this->mainLayout, CSTICK_PANE);

    for (int i = 0; i < kDpadState_Count; ++i) {
        assert(this->dpadPanes[i] != NULL && "Failed to load DPAD panes");
        lyt_setPaneVisible(this->dpadPanes[i], i == kDpadState_Off);
    }

    for (int i = 0; i < kAccelState_Count; ++i) {
        assert(this->accelPanes[i] != NULL && "Failed to load ACCEL panes");
        lyt_setPaneVisible(this->accelPanes[i], i == kAccelState_Off);
    }

    for (int i = 0; i < kTrigger_Count; ++i) {
        for (int j = 0; j < kTriggerState_Count; ++j) {
            assert(this->triggerPanes[i][j] != NULL && "Failed to load TRIGGER panes");
            bool visible = j == kTriggerState_Off;
            // Disable brakedrift panes in non-brakedrift mode
            if (!speedModIsEnabled)
                visible &= (i != kTrigger_BrakeDrift);
            lyt_setPaneVisible(this->triggerPanes[i][j], visible);
        }
    }

    assert(this->cstickPane != NULL && "Failed to load CSTICK pane");
    lyt_setPaneVisible(this->cstickPane, true);

    this->cstickOrigin = this->cstickPane->trans;

    this->playerId = CtrlRaceBase_getPlayerId(this);
}

static void CtrlRaceInputDisplay_setDPAD(CtrlRaceInputDisplay *this, DpadState state) {
    if (this->dpadState == state)
        return;

    // Only hold for off press
    if (state == kDpadState_Off && this->dpadTimer != 0 && --this->dpadTimer)
        return;
    lyt_setPaneVisible(this->dpadPanes[this->dpadState], false);
    lyt_setPaneVisible(this->dpadPanes[state], true);
    this->dpadState = state;
    this->dpadTimer = DPAD_HOLD_FOR_N_FRAMES;
}

static void CtrlRaceInputDisplay_setACCEL(CtrlRaceInputDisplay *this, AccelState state) {
    if (this->accelState == state)
        return;
    lyt_setPaneVisible(this->accelPanes[this->accelState], false);
    lyt_setPaneVisible(this->accelPanes[state], true);
    this->accelState = state;
}

static void CtrlRaceInputDisplay_setTRIGGER(
        CtrlRaceInputDisplay *this, Triggers trigger, TriggerState state) {
    if (this->triggerStates[trigger] == state)
        return;
    lyt_setPaneVisible(this->triggerPanes[trigger][this->triggerStates[trigger]], false);
    lyt_setPaneVisible(this->triggerPanes[trigger][state], true);
    this->triggerStates[trigger] = state;
}

static void CtrlRaceInputDisplay_setCSTICK(
        CtrlRaceInputDisplay *this, const Vec2 *state) {
    if (this->cstickState.x == state->x && this->cstickState.y == state->y)
        return;

    // Map range [-1, 1] -> [-width/2, width/2]
    const lyt_Pane *cstickPane = this->cstickPane;
    this->cstickPane->trans.x =
            this->cstickOrigin.x + 0.5f * state->x * cstickPane->scale.x * cstickPane->width;
    this->cstickPane->trans.y =
            this->cstickOrigin.y + 0.5f * state->y * cstickPane->scale.y * cstickPane->height;

    this->cstickState = *state;
}

static void CtrlRaceInputDisplay_calcSelf(UIControl *base) {
    CtrlRaceInputDisplay *this = (CtrlRaceInputDisplay *)base;

    CtrlRaceBase_process(this);

    // Update display
    u32 playerId = CtrlRaceBase_getPlayerId(this);
    if (playerId != this->playerId) {
        this->dpadTimer = 0;
        this->playerId = playerId;
    }

    assert(s_raceManager);
    assert(s_raceManager->players[playerId]);
    assert(s_raceManager->players[playerId]->padProxy);
    RaceInputState *input =
            &s_raceManager->players[playerId]->padProxy->currentInputState;

    assert(input->trick < kDpadState_Count);
    CtrlRaceInputDisplay_setDPAD(this, MIN(input->trick, kDpadState_Count - 1));
    CtrlRaceInputDisplay_setACCEL(this,
            (input->buttons & BUTTON_ACCEL) ? kAccelState_Pressed : kAccelState_Off);
    CtrlRaceInputDisplay_setTRIGGER(this, kTrigger_L,
            (input->buttons & BUTTON_ITEM) ? kTriggerState_Pressed : kTriggerState_Off);
    CtrlRaceInputDisplay_setTRIGGER(this, kTrigger_R,
            (input->buttons & (BUTTON_BRAKE | BUTTON_DRIFT)) ? kTriggerState_Pressed
                                                             : kTriggerState_Off);
    assert(input->stick.x <= 1.0f && input->stick.x >= -1.0f);
    assert(input->stick.y <= 1.0f && input->stick.y >= -1.0f);
    CtrlRaceInputDisplay_setCSTICK(this, &input->stick);

    // 200cc BrakeDrift
    if (speedModIsEnabled) {
        CtrlRaceInputDisplay_setTRIGGER(this, kTrigger_BrakeDrift,
                (input->buttons & BUTTON_BRAKEDRIFT) ? kTriggerState_Pressed
                                                     : kTriggerState_Off);
    }
}

static void CtrlRaceInputDisplay_draw(UIControl *base) {
    // Perform the check here to support hot-swapping in the future via an in-race license
    // settings editor.
    if (SaveManager_getSettingRaceInputDisplay(s_saveManager) !=
            SP_SETTING_RACE_INPUT_DISPLAY_SIMPLE) {
        return;
    }

    LayoutUIControl_draw(base);
};

static const CtrlRaceBase_vt s_CtrlRaceInputDisplay_vt = {
    .dt = CtrlRaceInputDisplay_dt,
    .init = LayoutUIControl_init,
    .calc = LayoutUIControl_calc,
    .draw = CtrlRaceInputDisplay_draw,
    .initSelf = CtrlRaceInputDisplay_initSelf,
    .calcSelf = CtrlRaceInputDisplay_calcSelf,
    .vf_20 = &UIControl_vf_20,
    .vf_24 = &UIControl_vf_24,
    .vf_28 = &CtrlRaceBase_vf_28,
    .vf_2c = &CtrlRaceBase_vf_2c,
    .vf_30 = &LayoutUIControl_vf_30,
    .vf_34 = &UIControl_vf_34,
    .vf_38 = &LayoutUIControl_vf_38,
    .process = CtrlRaceBase_process,
    .vf_40 = &CtrlRaceBase_vf_40,
    .vf_44 = &CtrlRaceBase_vf_44,
    .vf_48 = &CtrlRaceBase_vf_48,
    .vf_4c = &CtrlRaceBase_vf_4c,
};

void CtrlRaceInputDisplay_load(
        CtrlRaceInputDisplay *this, u32 localPlayerCount, u32 localPlayerId) {
    this->localPlayerId = localPlayerId;

    char variant[0x20];
    u32 variantId = localPlayerCount == 3 ? 4 : localPlayerCount;
    snprintf(variant, sizeof(variant), "InputDisplay_%lu_%lu", variantId, localPlayerId);

    const char *groups[] = {
        NULL,
        NULL,
        NULL,
    };
    LayoutUIControl_load(this, "game_image", "InputDisplay", variant, groups);
    // setAnimationInactive
}
