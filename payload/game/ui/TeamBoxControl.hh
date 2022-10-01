#pragma once

#include "game/ui/UIControl.hh"

namespace UI {

class TeamBoxControl : public LayoutUIControl {
public:
    TeamBoxControl();
    ~TeamBoxControl() override;

    void load();
    void refresh(u32 teamId, u32 positionId);
};

} // namespace UI
