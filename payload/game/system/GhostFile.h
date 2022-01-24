#pragma once

#include "Mii.h"

enum {
    MAX_GHOST_COUNT = 512,
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

    u8 _20[0x3c - 0x20];
    RawMii mii;
} RawGhostHeader;
static_assert(sizeof(RawGhostHeader) == 0x88);

enum {
    CTGP_FOOTER_MAGIC = 0x434b4744, // CKGD
    SP_FOOTER_MAGIC = 0x53504744, // SPGD
};

typedef struct {
    u32 size;
    u32 magic;
} FooterFooter;
static_assert(sizeof(FooterFooter) == 0x8);

typedef struct {
    u8 _00[0x48 - 0x00];
    u32 courseSha1[5];
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
    u32 courseSha1[5];
    f32 lapTimeDiffs[11];
    bool hasSpeedMod : 1;
    bool hasUltraShortcut : 1;
    bool hasHwg : 1;
    bool hasWallride : 1;
} SpFooter;

void SpFooter_onRaceStart(const u32 *courseSha1, bool speedModIsEnabled);

void SpFooter_onLapEnd(u32 lap, f32 timeDiff);

void SpFooter_onUltraShortcut(void);

void SpFooter_onHwg(void);

void SpFooter_onWallride(void);

typedef struct {
    u32 magic;
    union {
        CtgpFooter ctgp;
        SpFooter sp;
    };
} GhostFooter;

void GhostFooter_init(GhostFooter *footer, const u8 *raw, u32 size);

const u32 *GhostFooter_getCourseSha1(const GhostFooter *this);

bool GhostFooter_hasSpeedMod(const GhostFooter *this);

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
    bool headerIsValid;
    u8 _01[0x18 - 0x01];
    RawMii rawMii;
    u8 _64[0xa4 - 0x64];
    Time raceTime;
    u32 characterId;
    u32 vehicleId;
    u32 courseId;
    u8 _bc[0xcc - 0xbc];
    u8 country;
    u8 _cd[0xd0 - 0xcd];
    u32 inputsSize;
    u8 *inputs;
} GhostFile;
static_assert(sizeof(GhostFile) == 0xd8);

void GhostFile_writeHeader(const GhostFile *this, RawGhostHeader *header);

u32 GhostFile_spWrite(const GhostFile *this, u8 *raw);

typedef struct {
    u8 _00[0x04 - 0x00];
    u16 count;
    u8 _06[0x14 - 0x06];
} GhostGroup;
static_assert(sizeof(GhostGroup) == 0x14);

GhostGroup *GhostGroup_ct(GhostGroup *this);

void GhostGroup_invalidate(GhostGroup *this, u16 i);

GhostFile *GhostGroup_get(const GhostGroup *this, u16 i);

void GhostGroup_readHeader(GhostGroup *this, u16 i, RawGhostHeader *raw);
