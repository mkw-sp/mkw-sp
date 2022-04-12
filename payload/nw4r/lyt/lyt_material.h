#pragma once

#include "lyt_texMap.h"

#include <revolution.h>

typedef struct {
    u8 _00[0x10 - 0x00];
    GXColorS10 tevColors[3];
    u8 _28[0x5c - 0x28];
} lyt_Material;
static_assert(sizeof(lyt_Material) == 0x5c);

lyt_TexMap *lyt_Material_GetTexMapAry(lyt_Material *self);
