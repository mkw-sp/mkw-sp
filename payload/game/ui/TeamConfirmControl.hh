#pragma once

#include "game/ui/UIControl.hh"

namespace UI {

class TeamConfirmControl : public LayoutUIControl {
public:
    TeamConfirmControl();
    ~TeamConfirmControl() override;

    void load();
    void refresh(u32 playerId, u32 characterId, u32 colorId, u32 positionId);
};

} // namespace UI
