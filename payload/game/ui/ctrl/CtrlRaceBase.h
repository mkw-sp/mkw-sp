#pragma once

#include "../UIControl.h"

typedef struct {
    LayoutUIControl;
    u8 _174[0x190 - 0x174];
    u8 localPlayerId;
    u8 _191[0x198 - 0x191];
} CtrlRaceBase;

typedef struct {
    LayoutUIControl_vt;
    void (*process)(CtrlRaceBase *this);
    void *vf_40;
    void *vf_44;
    void *vf_48;
    void *vf_4c;
} CtrlRaceBase_vt;

void CtrlRaceBase_dt(UIControl *base, s32 type);

void CtrlRaceBase_initSelf(UIControl *base);

extern u8 CtrlRaceBase_vf_28;

extern u8 CtrlRaceBase_vf_2c;

void CtrlRaceBase_process(CtrlRaceBase *this);

extern u8 CtrlRaceBase_vf_40;

extern u8 CtrlRaceBase_vf_44;

extern u8 CtrlRaceBase_vf_48;

extern u8 CtrlRaceBase_vf_4c;

void CtrlRaceBase_setPaneColor(CtrlRaceBase *this, const char *pane, bool teamColors);

s8 CtrlRaceBase_getPlayerId(CtrlRaceBase *this);

void CtrlRaceBase_initLabelVisibility(CtrlRaceBase *this, u32 localPlayerCount, const char *pane);
