#pragma once

#include "game/ui/ctrl/CtrlRaceBase.hh"

namespace UI {

class CtrlRaceLap : public CtrlRaceBase {
public:
    CtrlRaceLap();
    ~CtrlRaceLap() override;
    void init() override;
    void REPLACED(calcSelf)();
    REPLACE void calcSelf() override;
    void vf_28() override;
    void vf_2c() override;

    void load(u32 localPlayerCount, u32 localPlayerId);

private:
    u8 _198[0x19b - 0x198];
    u8 m_screenCount; // Added (was padding)
};
static_assert(sizeof(CtrlRaceLap) == 0x19c);

} // namespace UI
