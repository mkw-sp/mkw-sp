#pragma once

#include "game/ui/ctrl/CtrlRaceBase.hh"

namespace UI {

class CtrlRaceWaitSymbol : public CtrlRaceBase {
public:
    CtrlRaceWaitSymbol();
    ~CtrlRaceWaitSymbol() override;
    void calcSelf() override;
    void vf_28() override;
    const char *getTypeName() override;
    void vf_40() override;
    void vf_44() override;
    bool vf_48() override;
    void vf_4c() override;

    void load(u32 localPlayerCount, u32 localPlayerId);

private:
    u8 _198[0x19c - 0x198];
};
static_assert(sizeof(CtrlRaceWaitSymbol) == 0x19c);

} // namespace UI
