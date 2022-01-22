#pragma once

#include <Common.h>

typedef struct {
    u8 _000[0x074 - 0x000];
    Vec3 internalVelDir;
    u8 _080[0x294 - 0x080];
} KartMove;
static_assert(sizeof(KartMove) == 0x294);
