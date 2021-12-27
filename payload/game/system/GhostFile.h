#pragma once

#include "../util/Mii.h"

enum {
    MAX_GHOST_COUNT = 512,
};

enum {
    GHOST_TYPE_NONE = 0x0,
    GHOST_TYPE_SAVED = 0x1,
    GHOST_TYPE_MAX = 0x26,
};

typedef struct {
    u8 _00[0x08 - 0x00];

    u32 vehicleId : 6;
    u32 characterId : 6;
    u32 year : 7;
    u32 month : 4;
    u32 day : 5;
    u32 controllerId : 4;

    u8 _pad1 : 4;
    bool isCompressed : 1;
    u8 _pad2 : 2;
    u32 type : 7;
    bool driftIsAuto : 1;
    u8 _pad3 : 1;

    u8 _0e[0x3c - 0x0e];
    RawMii mii;
} RawGhostHeader;

bool RawGhostHeader_isValid(RawGhostHeader *this);

bool RawGhostFile_decompress(const u8 *restrict src, u8 *restrict dst);

typedef struct {
    u8 _0[0x4 - 0x0];
    u16 minutes;
    u8 seconds;
    u16 milliseconds;
    u8 _a[0xc - 0xa];
} Time;

typedef struct {
    u8 _00[0x18 - 0x00];
    RawMii rawMii;
    u8 _64[0xa4 - 0x64];
    Time raceTime;
    u32 characterId;
    u32 vehicleId;
    u8 _b8[0xcc - 0xb8];
    u8 country;
    u8 _cd[0xd8 - 0xcd];
} GhostFile;

typedef struct {
    u8 _00[0x04 - 0x00];
    u16 count;
    u8 _06[0x14 - 0x06];
} GhostGroup;

GhostGroup *GhostGroup_ct(GhostGroup *this);

void GhostGroup_invalidate(GhostGroup *this, u16 i);

void GhostGroup_readHeader(GhostGroup *this, u16 i, RawGhostHeader *raw);
