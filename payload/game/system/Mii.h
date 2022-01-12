#pragma once

#include <Common.h>

typedef struct {
    u8 avatar[4];
    u8 client[4];
} MiiId;

typedef struct {
    u8 _00[0x4c - 0x00];
} RawMii;

typedef struct {
    u8 _00[0x68 - 0x00];
    wchar_t name[10];
    u8 _7c[0x94 - 0x7c];
    MiiId id;
    u8 _9c[0xb8 - 0x9c];
} Mii;
static_assert(sizeof(Mii) == 0xb8);
