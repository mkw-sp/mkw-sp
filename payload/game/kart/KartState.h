#pragma once

#include <Common.h>

typedef struct {
    u8 _00[0x08 - 0x00];
    u32 : 27;
    bool inCannon : 1;
    u32 : 4;
    u8 _0c[0xc0 - 0x0c];
} KartState;
static_assert(sizeof(KartState) == 0xc0);
