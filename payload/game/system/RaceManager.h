#pragma once

#include "InputManager.h"

typedef struct {
    u8 _0[0x8 - 0x0];
} TimeAttackGameMode;
static_assert(sizeof(TimeAttackGameMode) == 0x8);

bool TimeAttackGameMode_canEndRace(TimeAttackGameMode *this);

enum {
    RACE_MANAGER_PLAYER_FLAG_HAS_FINISHED = 1 << 1,
};

typedef struct {
    u8 _00[0x08 - 0x00];
    u8 id;
    u8 _09[0x38 - 0x09];
    u32 _pad0 : 30;
    bool hasFinished : 1;
    u32 _pad1 : 1;
    u8 _3c[0x48 - 0x3c];
    PadProxy *padProxy;
    u8 _4c[0x54 - 0x4c];
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
