#pragma once

#include <Common.h>

typedef struct {
    u8 _000[0x12b - 0x000];
    bool isGhost;
    u8 _12c[0xae8 - 0x12c];
} KartEffect;
static_assert(sizeof(KartEffect) == 0xae8);
