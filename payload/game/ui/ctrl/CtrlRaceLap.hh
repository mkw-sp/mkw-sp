#pragma once

#include "game/ui/ctrl/CtrlRaceBase.hh"

namespace UI {

class CtrlRaceLap : public CtrlRaceBase {
public:
    void REPLACED(calcSelf)();
    REPLACE void calcSelf() override;

    REPLACE void load(const char *variant, u32 localPlayerId);

private:
    u8 _198[0x19c - 0x198];
};
static_assert(sizeof(CtrlRaceLap) == 0x19c);

} // namespace UI
