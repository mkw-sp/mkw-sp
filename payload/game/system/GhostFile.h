#pragma once

#include "Mii.h"

#include <egg/core/eggHeap.h>

enum {
    MAX_GHOST_COUNT = 4096,
};

enum {
    GHOST_TYPE_NONE = 0x0,
    GHOST_TYPE_SAVED = 0x1,
    GHOST_TYPE_FAST_STAFF = 0x26,
    GHOST_TYPE_MAX = 0x26,
};

typedef struct __attribute__((packed)) {
    u16 minutes : 7;
    u8 seconds : 7;
    u16 milliseconds : 10;
} RawTime;
static_assert(sizeof(RawTime) == 0x3);

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

    u8 _20[0x34 - 0x20];
    u32 country;
    u8 _38[0x3c - 0x38];
    RawMii mii;
} RawGhostHeader;
static_assert(sizeof(RawGhostHeader) == 0x88);

void SPFooter_OnRaceStart(const u8 *courseSHA1, bool speedModIsEnabled);

void SPFooter_OnLapEnd(u32 lap, f32 timeDiff);

void SPFooter_OnUltraShortcut(void);

void SPFooter_OnHwg(void);

void SPFooter_OnWallride(void);

void SPFooter_OnShroom(u32 lap);

bool RawGhostFile_IsValid(const u8 *raw);

typedef struct {
    u8 _0[0x4 - 0x0];
    u16 minutes;
    u8 seconds;
    u16 milliseconds;
    u8 _a[0xc - 0xa];
} Time;
static_assert(sizeof(Time) == 0xc);

typedef struct {
    bool headerIsValid;
    u8 _01[0x18 - 0x01];
    RawMii rawMii;
    u8 _64[0xa4 - 0x64];
    Time raceTime;
    u32 characterId;
    u32 vehicleId;
    u32 courseId;
    u8 _bc[0xc0 - 0xbc];
    u8 year;
    u8 month;
    u8 day;
    u8 _c3[0xcc - 0xc3];
    u8 country;
    u8 _cd[0xd0 - 0xcd];
    u32 inputsSize;
    u8 *inputs;
} GhostFile;
static_assert(sizeof(GhostFile) == 0xd8);

void GhostFile_writeHeader(const GhostFile *self, RawGhostHeader *header);

u32 GhostFile_spWrite(const GhostFile *self, u8 *raw);

typedef struct {
    u8 _00[0x14 - 0x00];
} GhostGroup;
static_assert(sizeof(GhostGroup) == 0x14);

GhostGroup *GhostGroup_ct(GhostGroup *self, EGG_Heap *heap, u32 type);
