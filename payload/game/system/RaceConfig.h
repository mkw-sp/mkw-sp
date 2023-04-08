#pragma once

#include <Common.h>

enum {
    PLAYER_TYPE_LOCAL = 0x0,
    PLAYER_TYPE_GHOST = 0x3,
    PLAYER_TYPE_NONE = 0x5,
};

typedef struct {
    u8 _00[0x08 - 0x00];
    u32 vehicleId;
    u32 characterId;
    u32 type;
    u8 _14[0xd0 - 0x14];
    s32 controllerId;
    u8 _d4[0xf0 - 0xd4];
} RaceConfigPlayer;
static_assert(sizeof(RaceConfigPlayer) == 0xf0);

enum {
    ENGINE_CLASS_50CC = 0x0,
    ENGINE_CLASS_100CC = 0x1,
    ENGINE_CLASS_150CC = 0x2,
};

enum {
    GAME_MODE_OFFLINE_VS = 0x1,
    GAME_MODE_TIME_ATTACK = 0x2,
    GAME_MODE_BATTLE = 0x3,
};

enum {
    GAME_TYPE_TIME_ATTACK = 0x0,
};

enum {
    MODE_FLAG_MIRROR = 1 << 0,
    MODE_FLAG_TEAMS = 1 << 1,
};

typedef struct {
    u8 _000[0x004 - 0x000];
    u8 playerCount;
    u8 _005[0x008 - 0x005];
    RaceConfigPlayer players[12];
    u32 courseId;
    u32 engineClass;
    u32 gameMode;
    u32 gameType;
    u32 battleType;
    u32 cpuMode;
    u32 itemMode;
    u8 localPlayerIds[4];
    u32 cupId;
    u8 raceNumber;
    u8 lapCount;
    u8 _26;
    u8 _27;
    u32 modeFlags;
    u8 _b74[0xbec - 0xb74];
    u8 (*ghostBuffer)[11][0x2800]; // Modified
} RaceConfigScenario;
static_assert(sizeof(RaceConfigScenario) == 0xbf0);

typedef struct {
    u8 _0000[0x0020 - 0x0000];
    RaceConfigScenario raceScenario;
    RaceConfigScenario menuScenario;
    RaceConfigScenario awardsScenario;
    u8 ghostBuffers[2][11][0x2800]; // Modified
    u8 trackPackInfo[0xa4];
} RaceConfig;
static_assert(offsetof(RaceConfig, ghostBuffers[0][2]) == 0x73f0);

extern RaceConfig *s_raceConfig;

RaceConfig *RaceConfig_createInstance(void);

RaceConfig *RaceConfig_ct(RaceConfig *this);
