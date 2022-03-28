#pragma once

#include "egg/core/eggXfb.h"

typedef struct {
    EGG_Xfb *headXfb;
    u8 _04[0x10 - 0x04];
} EGG_XfbManager;
static_assert(sizeof(EGG_XfbManager) == 0x10);
