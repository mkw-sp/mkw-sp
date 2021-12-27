#pragma once

#include <Common.h>

typedef struct {
    u8 _0000[0x005c - 0x0000];
    u32 language;
    u8 _0060[0x1100 - 0x0060];
} SystemManager;

extern SystemManager *s_systemManager;
