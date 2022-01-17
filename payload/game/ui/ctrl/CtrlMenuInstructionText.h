#pragma once

#include "../UIControl.h"

typedef struct {
    LayoutUIControl;
} CtrlMenuInstructionText;

CtrlMenuInstructionText *CtrlMenuInstructionText_ct(CtrlMenuInstructionText *this);

void CtrlMenuInstructionText_dt(CtrlMenuInstructionText *this, s32 type);

void CtrlMenuInstructionText_load(CtrlMenuInstructionText *this);

void CtrlMenuInstructionText_setMessage(CtrlMenuInstructionText *this, s32 messageId, ExtendedMessageInfo *info);
