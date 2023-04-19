#pragma once

#include "../system/Mii.h"

#include <egg/core/eggHeap.h>

typedef struct {
    u8 _00[0x24 - 0x00];
} MiiTexture;

typedef struct {
    u8 _00[0x04 - 0x00];
    Mii **miis;
    MiiTexture *textures[7];
    u8 _08[0x98 - 0x24];
} MiiGroup;
static_assert(sizeof(MiiGroup) == 0x98);

Mii *MiiGroup_get(MiiGroup *this, u32 index);
