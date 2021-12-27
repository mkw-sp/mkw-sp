#pragma once

#include <Common.h>

typedef struct {
    u8 _00[0x4c - 0x00];
} RawMii;

typedef struct {
    u8 _00[0xb8 - 0x00];
} Mii;
