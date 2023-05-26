#pragma once

#include "game/system/SaveManager.hh"

#include <sp/TrackPackManager.hh>

namespace System {

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
        s32 controllerId;
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
        u32 courseId;
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

    void nextCourseIndex();
    bool outOfTracks() const;
    u32 getCourseIndex() const;
    bool isVanillaTracks() const;
    SP::TrackPackInfo &getPackInfo();
    SP::TrackPackInfo &emplacePackInfo();

    bool generateRandomCourses();
    bool generateOrderedCourses(u16 currentIdx);

    Scenario &raceScenario();
    Scenario &menuScenario();
    Scenario &awardsScenario();
    u8 (&ghostBuffers())[2][11][0x2800];
    bool isSameTeam(u32 p0, u32 p1) const;
    void applyEngineClass();
    void applyItemFreq();
    void applyCPUMode();

    void endRace();

    void REPLACED(init)();
    REPLACE void init();

    void REPLACED(clear)();
    REPLACE void clear();

    void REPLACED(initRace)();
    REPLACE void initRace();

    static RaceConfig *Instance();

    u32 m_selectedTrackPack;

private:
    REPLACE static void ConfigurePlayers(Scenario &scenario, u32 screenCount);

    std::tuple<SP::TrackGameMode, SP::ClientSettings::CourseSelection> getCourseSettings();

    u32 m_currentCourse;
    u8 _000c[0x0020 - 0x000c];
    Scenario m_raceScenario;
    Scenario m_menuScenario;
    Scenario m_awardsScenario;
    u8 m_ghostBuffers[2][11][0x2800]; // Modified
    SP::CircularBuffer<SP::TrackPackInfo, 32> m_courseOrder;

    static RaceConfig *s_instance;
};

// If you are changing this, make sure you update in RaceConfig.h
static_assert(sizeof(SP::CircularBuffer<SP::TrackPackInfo, 32>) == 0x1008);

} // namespace System
