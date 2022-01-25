#pragma once

#include <Common.h>

typedef struct GXColor GXColor;

struct GXColor {
    u8 r;
    u8 g;
    u8 b;
    u8 a;
};
static_assert(sizeof(GXColor) == 0x4);
