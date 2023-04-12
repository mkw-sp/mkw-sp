#pragma once

#include <Common.h>

typedef struct {
    u8 _0000[0x0058 - 0x0000];
    u32 aspectRatio;
    u32 language;
    u8 _0060[0x0084 - 0x0060];
    u32 matchingArea;
    u8 _0088[0x1100 - 0x0088];
} SystemManager;
static_assert(sizeof(SystemManager) == 0x1100);

extern SystemManager *s_systemManager;
