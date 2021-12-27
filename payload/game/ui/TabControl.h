#pragma once

#include "MenuInputManager.h"
#include "UIControl.h"

typedef struct {
    LayoutUIControl;
    ControlInputManager inputManager;
    u8 _1f8[0x234 - 0x1f8];
    u32 index;
    u8 _238[0x240 - 0x238];
} TabOptionButton; // C++ name: TabControl::OptionButton

TabOptionButton *TabOptionButton_ct(TabOptionButton *this);

void TabOptionButton_dt(TabOptionButton *this, s32 type);

void TabOptionButton_load(TabOptionButton *this, u32 i, const char *dir, const char *file, const char *variant, u32 playerFlags, bool r8, bool pointerOnly);

void TabOptionButton_setPlayerFlags(TabOptionButton *this, u32 playerFlags);

void TabOptionButton_setChosen(TabOptionButton *this, bool chosen);

void TabOptionButton_select(TabOptionButton *this, u32 localPlayerId);

void TabOptionButton_onSelect(TabOptionButton *this, u32 localPlayerId);
