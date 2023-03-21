#pragma once

#include "game/ui/ctrl/Button.hh"

namespace UI {

class OnlineTeamSelectButton : public PushButton {
public:
    OnlineTeamSelectButton();
    ~OnlineTeamSelectButton() override;

    void refresh(u32 teamId);
};

} // namespace UI
