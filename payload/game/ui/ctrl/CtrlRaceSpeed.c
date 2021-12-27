#include "CtrlRaceSpeed.h"

#include "../../kart/KartObjectManager.h"

enum {
    GROUP_ID_INT0 = 0x0,
    GROUP_ID_INT1 = 0x1,
    GROUP_ID_INT2 = 0x2,
    GROUP_ID_FRACT0 = 0x3,
    GROUP_ID_FRACT1 = 0x4,
};

static const CtrlRaceBase_vt s_CtrlRaceSpeed_vt;

CtrlRaceSpeed *CtrlRaceSpeed_ct(CtrlRaceSpeed *this) {
    LayoutUIControl_ct(this);
    this->vt = &s_CtrlRaceSpeed_vt;

    return this;
}

static void CtrlRaceSpeed_dt(UIControl *base, s32 type) {
    CtrlRaceSpeed *this = (CtrlRaceSpeed *)base;

    CtrlRaceBase_dt(this, 0);
    if (type > 0) {
        delete(this);
    }
}

static void CtrlRaceSpeed_initSelf(UIControl *base) {
    CtrlRaceSpeed *this = (CtrlRaceSpeed *)base;

    CtrlRaceBase_initSelf(this);

    const char *panes[] = {
        "speed_00",
        "speed_01",
        "speed_02",
        "speed_03",
        "speed_04",
        "coron",
        "speed_text",
    };
    for (u32 i = 0; i < ARRAY_SIZE(panes); i++) {
        CtrlRaceBase_setPaneColor(this, panes[i], true);
    }
}

static void CtrlRaceSpeed_calcSelf(UIControl *base) {
    CtrlRaceSpeed *this = (CtrlRaceSpeed *)base;

    CtrlRaceBase_process(this);

    u32 playerId = CtrlRaceBase_getPlayerId(this);
    f32 speed = KartObjectProxy_getSpeed(s_kartObjectManager->objects[playerId]);
    s32 integral = speed;
    u32 fractional = (speed >= 0.0f ? speed - integral : integral - speed) * 100.0f + 0.5f;
    if (integral > 999) {
        integral = 999;
        fractional = 99;
    } else if (integral < -99) {
        integral = -99;
        fractional = 99;
    }
    if (fractional > 99) {
        fractional = 99;
    }

    f32 int0;
    if (integral >= 100) {
        int0 = integral / 100;
    } else if (integral <= -10) {
        int0 = 10;
    } else {
        int0 = 11;
    }
    f32 int1;
    if (integral >= 10) {
        int1 = (integral / 10) % 10;
    } else if (integral <= -10) {
        int1 = -integral / 10;
    } else if (speed < 0.0f) {
        int1 = 10;
    } else {
        int1 = 11;
    }
    f32 int2;
    if (speed >= 0.0f) {
        int2 = integral % 10;
    } else {
        int2 = -integral % 10;
    }
    f32 fract0 = fractional / 10;
    f32 fract1 = fractional % 10;

    UIAnimator_setAnimationInactive(&this->animator, GROUP_ID_INT0, 0, int0);
    UIAnimator_setAnimationInactive(&this->animator, GROUP_ID_INT1, 0, int1);
    UIAnimator_setAnimationInactive(&this->animator, GROUP_ID_INT2, 0, int2);
    UIAnimator_setAnimationInactive(&this->animator, GROUP_ID_FRACT0, 0, fract0);
    UIAnimator_setAnimationInactive(&this->animator, GROUP_ID_FRACT1, 0, fract1);
}

static const CtrlRaceBase_vt s_CtrlRaceSpeed_vt = {
    .dt = CtrlRaceSpeed_dt,
    .init = LayoutUIControl_init,
    .calc = LayoutUIControl_calc,
    .draw = LayoutUIControl_draw,
    .initSelf = CtrlRaceSpeed_initSelf,
    .calcSelf = CtrlRaceSpeed_calcSelf,
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

void CtrlRaceSpeed_load(CtrlRaceSpeed *this, const char *variant, u32 localPlayerId) {
    this->localPlayerId = localPlayerId;

    const char *groups[] = {
        "eAFInt0",
        "texture_pattern_0_9_0",
        NULL,
        "eAFInt1",
        "texture_pattern_0_9_1",
        NULL,
        "eAFInt2",
        "texture_pattern_0_9_2",
        NULL,
        "eAFFract0",
        "texture_pattern_0_9_3",
        NULL,
        "eAFFract1",
        "texture_pattern_0_9_4",
        NULL,
        NULL,
    };
    LayoutUIControl_load(this, "game_image", "speed_number", variant, groups);
    for (u32 i = 0; i < 5; i++) {
        UIAnimator_setAnimationInactive(&this->animator, i, 0, 0.0f);
    }
}
