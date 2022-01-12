#pragma once

#include "../system/Mii.h"

typedef struct {
    s32 intVals[9];
    u32 messageIds[9];
    Mii *miis[9];
    u8 _6c[0xc4 - 0x6c];
} ExtendedMessageInfo;
