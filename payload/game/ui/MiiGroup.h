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

MiiGroup *MiiGroup_ct(MiiGroup *this);

void MiiGroup_dt(MiiGroup *this, s32 type);

void MiiGroup_init(MiiGroup *this, u32 count, u32 presetFlags, EGG_Heap *heap);

void MiiGroup_insertFromId(MiiGroup *this, u32 index, MiiId *id);

void MiiGroup_insertFromRaw(MiiGroup *this, u32 index, RawMii *raw);

Mii *MiiGroup_get(MiiGroup *this, u32 index);

void MiiGroup_copy(MiiGroup *dstGroup, const MiiGroup *srcGroup, u32 srcIndex, u32 dstIndex);

void MiiGroup_swap(MiiGroup *this, u32 i0, u32 i1);
