#pragma once

#include "../UIControl.h"

typedef struct {
    LayoutUIControl;
} CtrlMenuPageTitleText;
static_assert(sizeof(CtrlMenuPageTitleText) == 0x174);

CtrlMenuPageTitleText *CtrlMenuPageTitleText_ct(CtrlMenuPageTitleText *this);

void CtrlMenuPageTitleText_dt(CtrlMenuPageTitleText *this, s32 type);

void CtrlMenuPageTitleText_load(CtrlMenuPageTitleText *this, bool isOptions);

void CtrlMenuPageTitleText_setMessage(CtrlMenuPageTitleText *this, s32 messageId, ExtendedMessageInfo *info);
