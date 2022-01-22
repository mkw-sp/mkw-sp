#pragma once

#include "Mii.h"

enum {
    MAX_GHOST_COUNT = 512,
};

enum {
    GHOST_TYPE_NONE = 0x0,
    GHOST_TYPE_SAVED = 0x1,
    GHOST_TYPE_MAX = 0x26,
};

typedef struct __attribute__((packed)) {
    u16 minutes : 7;
    u8 seconds : 7;
    u16 milliseconds : 10;
} RawTime;

typedef struct {
    u32 magic;

    RawTime raceTime;
    u32 courseId : 6;
    u8 _pad0 : 2;

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

    u16 inputsSize;
    u8 lapCount;

    RawTime lapTimes[5];

    u8 _20[0x3c - 0x20];
    RawMii mii;
} RawGhostHeader;
static_assert(sizeof(RawGhostHeader) == 0x88);

typedef struct {
    u32 size;
    u32 magic;
} FooterFooter;
static_assert(sizeof(FooterFooter) == 0x8);

typedef struct {
    u8 _00[0x48 - 0x00];
    u32 trackSha1[5];
    u8 _5c[0x64 - 0x5c];
    f32 raceTimeDiff;
    u8 _68[0x80 - 0x68];
    f32 lapTimeDiffs[10]; // Reversed
    u8 _a8[0xc6 - 0xa8];
    u8 category;
    u8 version;
    FooterFooter;
} CtgpFooter;
static_assert(sizeof(CtgpFooter) == 0xd0);

typedef struct {
    u32 version;
    u32 trackSha1[5];
    f32 raceTimeDiff;
    f32 lapTimeDiffs[11];
    bool hasSpeedMod : 1;
    bool hasUltraShortcut : 1;
    bool hasHwg : 1;
} SpFooter;

bool RawGhostFile_isValid(const u8 *raw);

bool RawGhostFile_spIsValid(const u8 *raw, u32 size);

bool RawGhostFile_spDecompress(const u8 *restrict src, u8 *restrict dst);

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
