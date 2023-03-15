#pragma once

#include "../Page.h"
#include "../UIControl.h"

typedef struct {
    Page inherit;
    u8 _044[0x04c - 0x044];
    bool isReady; // Modified
    u8 _050[0x370 - 0x04d];
    LayoutUIControl ghostMessage;
} TimeAttackSplitsPage;
static_assert(sizeof(TimeAttackSplitsPage) == 0x4e4);
