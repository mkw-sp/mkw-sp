#pragma once

#include "game/ui/MiiGroup.hh"

namespace UI {

struct GlobalContext {
public:
    void onChangeLicense();

    u8 _000[0x12c - 0x000];
    u32 m_timeAttackCharacterId;
    u8 _130[0x13c - 0x130];
    u32 m_timeAttackVehicleId;
    u8 _140[0x238 - 0x140];
    MiiGroup m_localPlayerMiis;
    u8 _2d0[0x3bc - 0x2d0];
    u32 _3bc;
    u8 _3c0[0x3c4-0x3c0];
    u32 m_timeAttackGhostType;
    s32 m_timeAttackCourseId;
    s32 m_timeAttackLicenseId;
    u8 _3d0[0x510 - 0x3d0];
    u32 m_timeAttackGhostCount; // Added
    u32 m_timeAttackGhostIndices[11]; // Added
};
static_assert(offsetof(GlobalContext, m_timeAttackGhostCount) == 0x510);

} // namespace UI
