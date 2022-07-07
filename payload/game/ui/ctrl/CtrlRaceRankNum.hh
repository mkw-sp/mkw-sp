#pragma once

#include "game/ui/ctrl/CtrlRaceBase.hh"

namespace UI {

class CtrlRaceRankNum : public CtrlRaceBase {
public:
    void REPLACED(calcSelf)();
    REPLACE void calcSelf() override;

private:
    u8 _198[0x1a4 - 0x198];
};
static_assert(sizeof(CtrlRaceRankNum) == 0x1a4);

} // namespace UI
