#pragma once

#include "game/ui/ctrl/CtrlRaceBase.hh"

namespace UI {

class CtrlRaceBattleAddPoint : public CtrlRaceBase {
public:
    CtrlRaceBattleAddPoint();
    ~CtrlRaceBattleAddPoint() override;

    void initSelf() override;
    REPLACE void calcSelf() override;
    void vf_28() override;
    void vf_2c() override;

    void load(const char *variant, u32 screenId);

private:
    u32 m_score;
    s8 m_scoreDiffs[100];
    u32 m_scoreDiffCount;
    u32 m_frame;
};
static_assert(sizeof(CtrlRaceBattleAddPoint) == 0x208);

} // namespace UI
