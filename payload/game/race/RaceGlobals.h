#pragma once

typedef struct {
    u8 _00[0x04 - 0x00];
    bool isTimeAttack;
    u8 _05[0x20 - 0x05];
} RaceGlobals;
static_assert(sizeof(RaceGlobals) == 0x20);

extern RaceGlobals s_raceGlobals;
