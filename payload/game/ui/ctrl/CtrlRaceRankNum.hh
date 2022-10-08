#pragma once

#include "game/ui/ctrl/CtrlRaceBase.hh"

namespace UI {

class CtrlRaceRankNum : public CtrlRaceBase {
public:
    REPLACE void init() override;
    void REPLACED(calcSelf)();
    REPLACE void calcSelf() override;
    bool REPLACED(vf_48)();
    REPLACE bool vf_48() override;

    REPLACE void load(const char *variant, u32 localPlayerId);

private:
    bool isDisabled() const;

    u8 m_rank;
    u32 m_playerId;
    nw4r::lyt::Pane *_1a0;
};
static_assert(sizeof(CtrlRaceRankNum) == 0x1a4);

} // namespace UI
