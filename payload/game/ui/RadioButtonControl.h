#pragma once

#include "UIControl.h"

struct RadioButtonControl;

struct RadioButtonControlHandler;

typedef struct {
    u8 _0[0x8 - 0x0];
    void (*handle)(struct RadioButtonControlHandler *handler, struct RadioButtonControl *control, u32 localPlayerId, s32 selected);
} RadioButtonControlHandler_vt;

typedef struct RadioButtonControlHandler {
    const RadioButtonControlHandler_vt *vt;
} RadioButtonControlHandler;

typedef struct RadioButtonControl {
    LayoutUIControl;
    u8 _174[0x20c - 0x174];
    u32 index;
    u8 _210[0x298 - 0x210];
} RadioButtonControl;
static_assert(sizeof(RadioButtonControl) == 0x298);

RadioButtonControl *RadioButtonControl_ct(RadioButtonControl *this);

void RadioButtonControl_dt(RadioButtonControl *this, s32 type);

// TODO stack_f
void RadioButtonControl_load(RadioButtonControl *this, u32 buttonCount, u32 chosen, const char *dir, const char *file, const char *variant, const char *buttonFile, const char *const *buttonVariants, u32 playerFlags, bool stack_f, bool pointerOnly);

void RadioButtonControl_setFrontHandler(RadioButtonControl *this, RadioButtonControlHandler *handler);

void RadioButtonControl_setSelectHandler(RadioButtonControl *this, RadioButtonControlHandler *handler);

void RadioButtonControl_selectDefault(RadioButtonControl *this, u32 localPlayerId);

void RadioButtonControl_select(RadioButtonControl *this, u32 localPlayerId);
