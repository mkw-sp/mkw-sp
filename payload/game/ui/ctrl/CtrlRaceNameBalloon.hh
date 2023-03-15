#pragma once

#include "game/ui/UIControl.hh"

#include <memory>

namespace UI {

class CtrlRaceNameBalloon : public LayoutUIControl {
public:
    void calcVisibility();

private:
    REPLACE void refresh(u32 playerId);
    void refreshTextMiiName(u32 playerId);
    void refreshTextTime(u32 playerId, bool leadingZeroes);
    void refreshTextDate(u32 playerId);

    u8 _174[0x178 - 0x174];
    s32 m_playerId;
    u8 _17c[0x184 - 0x17c];
    nw4r::lyt::Pane *m_linePane;
};
static_assert(sizeof(CtrlRaceNameBalloon) == 0x188);

class BalloonManager {
public:
    REPLACE BalloonManager();
    ~BalloonManager();
    REPLACE void dt(s32 type);

    REPLACE void init(u8 localPlayerId);
    REPLACE void addNameControl(CtrlRaceNameBalloon *nameControl);
    void REPLACED(calc)();
    REPLACE void calc();

private:
    u8 _00[0x03 - 0x00];
    u8 m_localPlayerId;
    u32 m_nameCount = 0;
    u8 _08[0x14 - 0x08];
    s32 m_playerIds[12];
    bool m_nameIsEnabled[12];                // Moved
    std::unique_ptr<Vec3[]> m_namePositions; // Modified
    u8 _54[0xc4 - 0x54];
};
static_assert(sizeof(BalloonManager) == 0xc4);

} // namespace UI
