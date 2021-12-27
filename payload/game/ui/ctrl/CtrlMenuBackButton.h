#pragma once

#include "../Button.h"

typedef struct {
    PushButton;
    u32 messageId;
    u32 fadeoutState;
    f32 fadeoutOpacity;
    u8 _260[0x264 - 0x260];
} CtrlMenuBackButton;

CtrlMenuBackButton *CtrlMenuBackButton_ct(CtrlMenuBackButton *this);

void CtrlMenuBackButton_dt(CtrlMenuBackButton *this, s32 type);
