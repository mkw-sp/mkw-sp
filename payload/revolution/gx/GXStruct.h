#pragma once

#include <Common.h>

typedef struct {
    u8 r;
    u8 g;
    u8 b;
    u8 a;
} GXColor;
static_assert(sizeof(GXColor) == 0x4);
