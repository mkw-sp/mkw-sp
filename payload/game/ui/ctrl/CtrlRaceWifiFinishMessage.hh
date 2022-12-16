#pragma once

#include "game/ui/ctrl/CtrlRaceBase.hh"

namespace UI {

class CtrlRaceWifiFinishMessage : public CtrlRaceBase {
public:
    CtrlRaceWifiFinishMessage();
    ~CtrlRaceWifiFinishMessage() override;
    void calcSelf() override;
    void vf_28() override;
    void vf_2c() override;
    void vf_40() override;
    void vf_44() override;
    bool vf_48() override;
    void vf_4c() override;

    void load(u32 localPlayerCount, u32 localPlayerId);

private:
    u8 _198[0x1a0 - 0x198];
};
static_assert(sizeof(CtrlRaceWifiFinishMessage) == 0x1a0);

} // namespace UI
