#pragma once

#include "UIControl.h"

typedef struct {
    LayoutUIControl;
} MessageWindowControl;

MessageWindowControl *MessageWindowControl_ct(MessageWindowControl *this);

void MessageWindowControl_dt(MessageWindowControl *this, s32 type);

void MessageWindowControl_load(MessageWindowControl *this, const char *dir, const char *file,
        const char *variant);
