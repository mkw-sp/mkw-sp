#pragma once

#include "game/ui/ctrl/CtrlRaceBase.hh"

namespace UI {

class CtrlRaceRankNum : public CtrlRaceBase {
public:
    void REPLACED(calcSelf)();
    REPLACE void calcSelf() override;
    bool REPLACED(vf_48)();
    REPLACE bool vf_48() override;

private:
    bool isDisabled() const;

    u8 _198[0x1a4 - 0x198];
};
static_assert(sizeof(CtrlRaceRankNum) == 0x1a4);

} // namespace UI
