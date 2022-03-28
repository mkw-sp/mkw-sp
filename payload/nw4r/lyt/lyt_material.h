#pragma once

#include "lyt_texMap.h"

typedef struct {
    u8 _00[0x5c - 0x00];
} lyt_Material;
static_assert(sizeof(lyt_Material) == 0x5c);

lyt_TexMap *lyt_Material_GetTexMapAry(lyt_Material *self);
