#pragma once

#include <Common.h>

enum {
    PATCHER_BINARY_NONE = 0x0,
    PATCHER_BINARY_DOL = 0x1,
    PATCHER_BINARY_REL = 0x2,
};

void Patcher_patch(u32 binary);
