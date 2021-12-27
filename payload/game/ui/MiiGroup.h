#pragma once

#include "../util/Mii.h"

#include <egg/core/eggHeap.h>

typedef struct {
    u8 _00[0x98 - 0x00];
} MiiGroup;

MiiGroup *MiiGroup_ct(MiiGroup *this);

void MiiGroup_dt(MiiGroup *this, s32 type);

void MiiGroup_init(MiiGroup *this, u32 count, u32 presetFlags, EGG_Heap *heap);

void MiiGroup_load(MiiGroup *this, u32 index, RawMii *raw);

Mii *MiiGroup_get(MiiGroup *this, u32 index);

void MiiGroup_copy(MiiGroup *dstGroup, const MiiGroup *srcGroup, u32 srcIndex, u32 dstIndex);
