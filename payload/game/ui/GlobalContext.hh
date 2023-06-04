#pragma once

#include "game/ui/MiiGroup.hh"
#include "game/util/Registry.hh"

namespace UI {

enum class OnlineErrorCategory {
    ErrorCode = 0x1,
    MiiInvalid = 0x2,
    GeneralDisconnect = 0x4,
    UnrecoverableDisconnect = 0x5,
};

class GlobalContext {
public:
    void copyPlayerMiis();
    void REPLACED(onChangeLicense)();
    REPLACE void onChangeLicense();

    void applyVehicleRestriction(bool isBattle);

    struct SelectPlayer {
        u32 m_characterId;
        u32 m_vehicleId;
        u8 _08;
    };

    struct OnlineDisconnectInfo {
        OnlineErrorCategory m_category;
        u32 m_errorCode;
    };

    enum class VehicleRestriction : u32 { KartsOnly = 0, BikesOnly = 1, All = 2 };

    u8 _000[0x060 - 0x000];
    u32 m_match;
    u32 m_matchCount;
    u8 _068[0x074 - 0x068];
    VehicleRestriction m_vehicleRestriction;
    u8 _078[0x124 - 0x078];
    u32 m_localPlayerCount;
    u8 _128[0x12c - 0x128];
    Registry::Character m_localCharacterIds[4];
    Registry::Vehicle m_localVehicleIds[4];
    u32 m_raceCourseId;
    u32 m_battleCourseId;
    u8 _154[0x164 - 0x154];
    u32 m_driftModes[4];
    u8 _174[0x188 - 0x174];
    MiiGroup m_playerMiis;
    SelectPlayer m_selectPlayer[2];
    MiiGroup m_localPlayerMiis;
    u8 _2d0[0x3bc - 0x2d0];
    u32 _3bc;
    u8 _3c0[0x3c4 - 0x3c0];
    u32 m_timeAttackGhostType;
    Registry::Course m_timeAttackCourseId;
    s32 m_timeAttackLicenseId;
    u8 _3d0[0x4c8 - 0x3d0];
    u32 m_lastTitleBackground;
    u8 _4cc[0x500 - 0x4cc];
    OnlineDisconnectInfo m_onlineDisconnectInfo;
    u8 _508[0x510 - 0x508];
    u32 m_timeAttackGhostCount;       // Added
    u32 m_timeAttackGhostIndices[11]; // Added
};

// Keep in sync with SectionManager.c
static_assert(sizeof(GlobalContext) == 0x510 + sizeof(u32) * (1 + 11));

} // namespace UI
