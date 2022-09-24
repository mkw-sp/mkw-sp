#pragma once

#include "game/ui/UIControl.hh"

namespace UI {

class CtrlRaceResultTeam : public LayoutUIControl {
public:
    CtrlRaceResultTeam();
    ~CtrlRaceResultTeam() override;

    void load();
    void refresh(u32 playerId, u32 characterId, u32 teamId, u32 positionId);
    void refresh(u32 score);
};

} // namespace UI
