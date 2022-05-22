#pragma once

#include "game/ui/MiiGroup.hh"

namespace UI {

struct GlobalContext {
public:
    void onChangeLicense();

    u8 _000[0x238 - 0x000];
    MiiGroup m_localPlayerMiis;
    u8 _2d0[0x510 - 0x2d0];
    u32 m_timeAttackGhostCount; // Added
    u32 m_timeAttackGhostIndices[11]; // Added
};
static_assert(offsetof(GlobalContext, m_timeAttackGhostCount) == 0x510);

} // namespace UI
