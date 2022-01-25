#pragma once

#include <Common.h>

typedef struct {
    u8 avatar[4];
    u8 client[4];
} MiiId;
static_assert(sizeof(MiiId) == 0x8);

typedef struct {
    u8 _00[0x4a - 0x00];
    u16 crc16;
} RawMii;
static_assert(sizeof(RawMii) == 0x4c);

typedef struct {
    u8 _00[0x68 - 0x00];
    wchar_t name[10];
    u8 _7c[0x94 - 0x7c];
    MiiId id;
    u8 _9c[0xb8 - 0x9c];
} Mii;
static_assert(sizeof(Mii) == 0xb8);
