#pragma once

#include <Common.h>

typedef struct {
    u16 width;
    u16 height;
    void *buffer;
    u8 _08[0x10 - 0x08];
} EGG_Xfb;
static_assert(sizeof(EGG_Xfb) == 0x10);

u32 EGG_Xfb_calcXfbSize(u16 width, u16 height);
