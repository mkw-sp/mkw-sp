#pragma once

#include <Common.h>

typedef struct {
    u8 _0[0x8 - 0x0];
} TimeAttackGameMode;
static_assert(sizeof(TimeAttackGameMode) == 0x8);

bool TimeAttackGameMode_canEndRace(TimeAttackGameMode *this);

enum {
    RACE_MANAGER_PLAYER_FLAG_HAS_FINISHED = 1 << 1,
};
