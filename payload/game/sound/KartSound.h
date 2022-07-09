#pragma once

#include <Common.h>

typedef struct {
    u8 _00[0xe0 - 0x00];
    bool isLocal;
    bool isGhost;
    u8 _e2[0xfc - 0xe2];
} KartSound;
static_assert(sizeof(KartSound) == 0xfc);
