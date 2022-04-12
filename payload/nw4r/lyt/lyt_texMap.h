#pragma once

#include <Common.h>

typedef struct {
    u8 _00[0x1c - 0x00];
} lyt_TexMap;
static_assert(sizeof(lyt_TexMap) == 0x1c);

void lyt_TexMap_ReplaceImage(lyt_TexMap *self, void *tpl, u32 index);
