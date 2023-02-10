#pragma once

#include "game/ui/ctrl/CtrlRaceBase.hh"

namespace UI {

class CtrlRaceBattleAddPoint : public CtrlRaceBase {
public:
    REPLACE void calcSelf() override;

private:
    u32 m_score;
    s8 m_scoreDiffs[100];
    u32 m_scoreDiffCount;
    u32 m_frame;
};
static_assert(sizeof(CtrlRaceBattleAddPoint) == 0x208);

} // namespace UI
