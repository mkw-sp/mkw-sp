#pragma once

#include "game/ui/UIControl.hh"

namespace UI {

class CtrlRaceResult : public LayoutUIControl {
public:
    CtrlRaceResult();
    ~CtrlRaceResult() override;
    // ...
    virtual void vf_3c();
    virtual void vf_40();
    REPLACE virtual const char *getFileName();

    void REPLACED(refreshRankIconsColor)(u32 rank, u32 playerId);
    REPLACE void refreshRankIconsColor(u32 rank, u32 playerId);

private:
    u8 _174[0x198 - 0x174];
};
static_assert(sizeof(CtrlRaceResult) == 0x198);

} // namespace UI
