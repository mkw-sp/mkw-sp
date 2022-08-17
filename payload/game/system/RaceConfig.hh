#pragma once

#include <Common.hh>

namespace System {

class RaceConfig {
public:
    struct Player {
        enum class Type {
            Local = 0,
            CPU = 1,
            Ghost = 3,
            None = 5,
        };

        u8 _00[0x04 - 0x00];
        u8 spTeam; // Added (was unused, but not padding)
        u8 _05[0x08 - 0x05];
        u32 vehicleId;
        u32 characterId;
        Type type;
        u8 _14[0xcc - 0x14];
        u32 team;
        s32 controllerId;
        u8 _d4[0xf0 - 0xd4];
    };
    static_assert(sizeof(Player) == 0xf0);

    enum class EngineClass {
        CC50 = 0,
        CC100 = 1,
        CC150 = 2,
    };

    enum class GameMode {
        OfflineVS = 1,
        TimeAttack = 2,
        OfflineBT = 3,
        Mission = 4,
        OnlinePrivateVS = 7,
        OnlinePrivateBT = 10,
    };

    struct Scenario {
        u8 _000[0x004 - 0x000];
        u8 playerCount;
        u8 _005[0x006 - 0x005];
        u8 localPlayerCount;
        u8 _007[0x008 - 0x007];
        Player players[12];
        u32 courseId;
        EngineClass engineClass;
        GameMode gameMode;
        u32 cameraMode;
        u32 battleType;
        u32 cpuMode;
        u32 itemMode;
        u8 localPlayerIds[4];
        u32 cupId;
        u8 raceNumber;
        u8 lapCount;
        u8 _26;
        u8 _27;
        u8 spMaxTeamSize : 3;
        u32 _ : 27;
        bool teams : 1;
        bool mirror : 1;
        u8 _b74[0xbec - 0xb74];
        u8 (*ghostBuffer)[11][0x2800]; // Modified
    };
    static_assert(sizeof(Scenario) == 0xbf0);

    Scenario &raceScenario();
    Scenario &menuScenario();
    Scenario &awardsScenario();
    u8 (&ghostBuffers())[2][11][0x2800];

    static RaceConfig *Instance();

private:
    u8 _0000[0x0020 - 0x0000];
    Scenario m_raceScenario;
    Scenario m_menuScenario;
    Scenario m_awardsScenario;
    u8 m_ghostBuffers[2][11][0x2800]; // Modified

    static RaceConfig *s_instance;
};

} // namespace System
