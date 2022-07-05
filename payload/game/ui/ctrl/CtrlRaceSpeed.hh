#pragma once

#include "game/ui/ctrl/CtrlRaceBase.hh"

namespace UI {

class CtrlRaceSpeed : public CtrlRaceBase {
public:
    CtrlRaceSpeed();
    ~CtrlRaceSpeed() override;
    void initSelf() override;
    void calcSelf() override;

    void load(u32 localPlayerCount, u32 localPlayerId);
};

} // namespace UI
