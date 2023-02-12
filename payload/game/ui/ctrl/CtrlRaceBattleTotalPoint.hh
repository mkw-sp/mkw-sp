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
    u32 m_teamCount;
    std::array<u32, 6> m_teamScores;
    std::array<u32, 6> m_teamIds;
    std::array<u32, 6> m_scores;
};

} // namespace UI
