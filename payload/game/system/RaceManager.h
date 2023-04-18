#pragma once

#include <Common.h>

typedef struct {
    u8 _00[0x08 - 0x00];
    u8 id;
    u8 _09[0x38 - 0x09];
    u32 _pad0 : 30;
    bool hasFinished : 1;
    u32 _pad1 : 1;
    u8 _3c[0x54 - 0x3c];
} RaceManagerPlayer;
static_assert(sizeof(RaceManagerPlayer) == 0x54);

RaceManagerPlayer *RaceManagerPlayer_ct(RaceManagerPlayer *this, u8 id, u8 lapCount);

typedef struct {
    u8 _00[0x0c - 0x00];
    RaceManagerPlayer **players;
    u8 _10[0x50 - 0x10];
} RaceManager;
static_assert(sizeof(RaceManager) == 0x50);

extern RaceManager *s_raceManager;
