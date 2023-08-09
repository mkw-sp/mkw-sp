#pragma once

#include "Mii.h"

#include <egg/core/eggHeap.h>

enum {
    GHOST_TYPE_PERSONAL_BEST = 0x01,
    GHOST_TYPE_WORLD_CHAMPION = 0x02,
    GHOST_TYPE_CONTINENTAL_CHAMPION = 0x03,
    GHOST_TYPE_RIVAL = 0x04,
    GHOST_TYPE_SPECIAL = 0x05,
    GHOST_TYPE_RACE = 0x06,
    GHOST_TYPE_NORMAL_STAFF = 0x25,
    GHOST_TYPE_EXPERT_STAFF = 0x26,
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
    u8 country;
    u8 _38[0x3c - 0x35];
    RawMii mii;
} RawGhostHeader;
static_assert(sizeof(RawGhostHeader) == 0x88);

void SPFooter_OnLapEnd(u32 lap, f32 timeDiff);

void SPFooter_OnUltraShortcut(void);

void SPFooter_OnHwg(void);

void SPFooter_OnWallride(void);

void SPFooter_OnShroom(u32 lap);

bool RawGhostFile_IsValid(const u8 *raw);

typedef struct {
    u8 _00[0x14 - 0x00];
} GhostGroup;
static_assert(sizeof(GhostGroup) == 0x14);

GhostGroup *GhostGroup_ct(GhostGroup *self, EGG_Heap *heap, u32 type);
