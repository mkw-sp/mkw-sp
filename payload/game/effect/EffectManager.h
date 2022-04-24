#pragma once

#include "game/effect/KartEffect.h"

typedef struct {
    u8 _000[0x068 - 0x000];
    KartEffect **karts;
    u8 _06c[0x9f4 - 0x06c];
} EffectManager;
static_assert(sizeof(EffectManager) == 0x9f4);

extern EffectManager *s_effectManager;
