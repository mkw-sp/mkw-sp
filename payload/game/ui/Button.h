#pragma once

#include "UIControl.h"

struct PushButton;

struct PushButtonHandler;

typedef struct {
    u8 _0[0x8 - 0x0];
    void (*handle)(struct PushButtonHandler *handler, struct PushButton *button, u32 localPlayerId);
} PushButtonHandler_vt;
static_assert(sizeof(PushButtonHandler_vt) == 0xc);

typedef struct PushButtonHandler {
    const PushButtonHandler_vt *vt;
} PushButtonHandler;

typedef struct {
    LayoutUIControl_vt;
    u8 _03c[0x4c - 0x3c];
} PushButton_vt;
static_assert(sizeof(PushButton_vt) == 0x4c);

typedef struct PushButton {
    LayoutUIControl inherit;
    u8 _174[0x240 - 0x174];
    s32 index;
    u8 _244[0x254 - 0x244];
} PushButton;
static_assert(sizeof(PushButton) == 0x254);

PushButton *PushButton_ct(PushButton *this);

void PushButton_dt(PushButton *this, s32 type);

// TODO r8
void PushButton_load(PushButton *this, const char *dir, const char *file, const char *variant,
        u32 playerFlags, bool r8, bool pointerOnly);

void PushButton_setFrontHandler(PushButton *this, PushButtonHandler *handler, bool repeat);

void PushButton_setSelectHandler(PushButton *this, PushButtonHandler *handler);

void PushButton_selectDefault(PushButton *this, u32 localPlayerId);

void PushButton_select(PushButton *this, u32 localPlayerId);

void PushButton_setPlayerFlags(PushButton *this, u32 playerFlags);

f32 PushButton_getDelay(PushButton *this);

void PushButton_setPressSound(PushButton* this, u32 pressSound);