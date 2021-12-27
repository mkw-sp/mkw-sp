#pragma once

#include "MenuInputManager.h"
#include "UIControl.h"

typedef struct {
    LayoutUIControl_vt;
    u8 _03c[0x4c - 0x3c];
} PushButton_vt;

typedef struct {
    LayoutUIControl;
    u8 _174[0x254 - 0x174];
} PushButton;

PushButton *PushButton_ct(PushButton *this);

void PushButton_dt(PushButton *this, s32 type);

// TODO r8
void PushButton_load(PushButton *this, const char *dir, const char *file, const char *variant, u32 playerFlags, bool r8, bool pointerOnly);

void PushButton_setFrontHandler(PushButton *this, InputHandler *handler, bool repeat);

void PushButton_setSelectHandler(PushButton *this, InputHandler *handler);

void PushButton_selectDefault(PushButton *this, u32 localPlayerId);

void PushButton_select(PushButton *this, u32 localPlayerId);
