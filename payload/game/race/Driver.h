#pragma once

#include "../snd/DriverSound.h"

typedef struct {
    u8 _000[0x00c - 0x000];
    DriverSound *sound;
    u8 _010[0x6e4 - 0x010];
} Driver;
static_assert(sizeof(Driver) == 0x6e4);
