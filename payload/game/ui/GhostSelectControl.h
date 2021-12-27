#pragma once

#include "GhostSelectButton.h"

typedef struct {
    LayoutUIControl;
    GhostSelectButton buttons[6];
} GhostSelectControl;

GhostSelectControl *GhostSelectControl_ct(GhostSelectControl *this);

void GhostSelectControl_dt(UIControl *base, s32 type);

void GhostSelectControl_load(GhostSelectControl *this);

bool GhostSelectControl_isHidden(GhostSelectControl *this);

bool GhostSelectControl_isShown(GhostSelectControl *this);

void GhostSelectControl_hide(GhostSelectControl *this);

void GhostSelectControl_slideInFromLeft(GhostSelectControl *this);

void GhostSelectControl_slideInFromRight(GhostSelectControl *this);

void GhostSelectControl_show(GhostSelectControl *this);

void GhostSelectControl_slideOutToLeft(GhostSelectControl *this);

void GhostSelectControl_slideOutToRight(GhostSelectControl *this);
