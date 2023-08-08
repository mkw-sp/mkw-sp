#pragma once

#include "game/sound/SoundId.hh"
#include "game/util/Registry.hh"

#include <sp/FixedString.hh>
#include <sp/ShaUtil.hh>

namespace System {

/// Configuration for the current race.
///
/// For configuration over a set of VS/BT races, see GlobalContext.
class RaceConfig {
public:
    RaceConfig();
    virtual ~RaceConfig();
    virtual void dt(s32 type);

    struct Player {
        enum class Type {
            Local = 0,
            CPU = 1,
            Ghost = 3,
            Online = 4,
            None = 5,
        };

        u8 _00[0x04 - 0x00];
        u8 spTeam; // Added (was unused, but not padding)
        s8 screenId;
        s8 ghostProxyId;
        u8 _07[0x08 - 0x07];
        u32 vehicleId;
        u32 characterId;
        Type type;
        u8 _14[0xcc - 0x14];
        u32 team;
        Registry::Controller controllerId;
        u8 _d4[0xd8 - 0xd4];
        u16 prevScore;
        u16 score;
        u8 _dc[0xe0 - 0xdc];
        u8 rank;
        u8 _e1[0xf0 - 0xe1];
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
        Awards = 11,
    };

    struct SPScenario {
        // The course path to load instead of the vanilla course.
        // If this is empty, the vanilla course will be loaded.
        SP::FixedString<64> pathReplacement;
        // The background music to load instead of the course ID's music.
        std::optional<Sound::SoundId> musicReplacement;
        // The course hash to use for ghost lookup instead of the slot's hash.
        std::optional<Sha1> courseSha;
        // The course name to use instead of the slot's name.
        // If this is empty, the slot's name will be used.
        SP::WFixedString<48> nameReplacement;
    };

    struct Scenario {
        bool isOnline() const {
            return gameMode >= GameMode::OnlinePrivateVS && gameMode <= GameMode::OnlinePrivateBT;
        }

        bool isVs() const {
            switch (gameMode) {
            case GameMode::OfflineVS:
            case GameMode::TimeAttack:
                return true;
            default:
                return false;
            }
        }

        bool isBattle() const {
            switch (gameMode) {
            case GameMode::OfflineBT:
                return true;
            default:
                return false;
            }
        }

        u8 _000[0x004 - 0x000];
        u8 playerCount;
        u8 screenCount;
        u8 localPlayerCount;
        u8 _007[0x008 - 0x007];
        Player players[12];
        Registry::Course courseId;
        EngineClass engineClass;
        GameMode gameMode;
        u32 cameraMode;
        u32 battleType;
        u32 cpuMode;
        u32 itemMode;
        s8 screenPlayerIds[4];
        u32 cupId;
        u8 raceNumber;
        u8 lapCount;
        u8 _26;
        u8 _27;
        u8 spMaxTeamSize : 3;
        bool mirrorRng : 1;
        u32 draw : 3;
        u32 _ : 21;
        bool is200cc : 1;
        bool competition : 1;
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
    bool isSameTeam(u32 p0, u32 p1) const;
    void applyEngineClass();
    void applyItemFreq();
    void applyPlayers();
    void applyCPUMode();
    void endRace();

    REPLACE void initRace();
    REPLACE void initAwards();
    REPLACE void initCredits();

private:
    void REPLACED(initRace)();
    void REPLACED(initAwards)();
    void REPLACED(initCredits)();

public:
    REPLACE static RaceConfig *CreateInstance();
    static RaceConfig *Instance();

private:
    REPLACE static void ConfigurePlayers(Scenario &scenario, u32 screenCount);

    u8 _0004[0x0020 - 0x0004];

public:
    Scenario m_raceScenario;
    Scenario m_menuScenario;
    Scenario m_awardsScenario;

    SPScenario m_spRace;
    SPScenario m_spMenu;
    SPScenario m_spAwards;

private:
    u8 m_ghostBuffers[2][11][0x2800]; // Modified

    static RaceConfig *s_instance;
};

extern "C" void RaceConfigScenario_resetGhostPlayerTypes(RaceConfig::Scenario *self);

} // namespace System
