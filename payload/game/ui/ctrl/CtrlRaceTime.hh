#pragma once

#include "game/ui/ctrl/CtrlRaceBase.hh"

namespace UI {

class CtrlRaceTime : public CtrlRaceBase {
public:
    void REPLACED(calcSelf)();
    REPLACE void calcSelf() override;

    REPLACE s8 getPlayerId();

private:
    u8 _198[0x1c4 - 0x198];
};
static_assert(sizeof(CtrlRaceTime) == 0x1c4);

} // namespace UI
