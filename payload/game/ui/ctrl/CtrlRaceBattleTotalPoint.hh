#pragma once

#include "game/ui/ctrl/CtrlRaceBase.hh"

namespace UI {

class CtrlRaceBattleTotalPoint : public CtrlRaceBase {
public:
    CtrlRaceBattleTotalPoint();
    ~CtrlRaceBattleTotalPoint() override;

    REPLACE void init() override;
    REPLACE void calcSelf() override;
    void vf_28() override;
    void vf_2c() override;

    REPLACE void load();

private:
    u32 m_scores[6]; // Replaced
    u32 m_playerCount; // Replaced
    u32 m_teamCount; // Replaced
};
static_assert(sizeof(CtrlRaceBattleTotalPoint) == 0x1b8);

} // namespace UI
