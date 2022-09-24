#pragma once

#include "game/ui/UIControl.hh"

namespace UI {

class CtrlRaceResultTeamPoint : public LayoutUIControl {
public:
    CtrlRaceResultTeamPoint();
    ~CtrlRaceResultTeamPoint() override;

    void load();
    void refresh(u32 teamId, u32 positionId);
    void refresh(u32 score);

private:
    u32 m_teamId = 0;
};

} // namespace UI
