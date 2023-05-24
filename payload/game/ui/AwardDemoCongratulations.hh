#pragma once

#include "game/ui/UIControl.hh"

namespace UI {

class AwardDemoCongratulations : public LayoutUIControl {
public:
    AwardDemoCongratulations();
    virtual ~AwardDemoCongratulations();
    void initSelf() override;
    void vf_28() override;
    const char *getTypeName() override;

    void load(bool isWin, bool isDraw, bool isTeams);
};

} // namespace UI
