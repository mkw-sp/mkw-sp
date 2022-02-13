#pragma once

#include "Page.h"

typedef struct {
    Page;
    u8 _044[0x604 - 0x044];
} MessageWindowPopupPage;
static_assert(sizeof(MessageWindowPopupPage) == 0x604);

void MessageWindowPopupPage_reset(MessageWindowPopupPage *this);

void MessageWindowPopupPage_setMessage(MessageWindowPopupPage *this, u32 messageId,
        MessageInfo *info);
