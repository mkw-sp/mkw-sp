#pragma once

#include "game/ui/ctrl/CtrlRaceBase.hh"

namespace UI {

class CtrlRaceBattlePoint : public CtrlRaceBase {
public:
    REPLACE void initSelf() override;
    REPLACE void calcSelf() override;

private:
    void refresh(u32 score);

    u32 m_score;
};
static_assert(sizeof(CtrlRaceBattlePoint) == 0x19c);

} // namespace UI
