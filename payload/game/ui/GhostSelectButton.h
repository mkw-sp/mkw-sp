#pragma once

#include "TabControl.h"

typedef struct {
    TabOptionButton;
    MiiGroup miiGroup;
    InputHandler onSelect;
    InputHandler onFront;
    bool chosen;
} GhostSelectButton;

GhostSelectButton *GhostSelectButton_ct(GhostSelectButton *this);

void GhostSelectButton_dt(GhostSelectButton *this, s32 type);

void GhostSelectButton_load(GhostSelectButton *this, u32 i);

void GhostSelectButton_refresh(GhostSelectButton *this, u32 ghostIndex);
