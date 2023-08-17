#pragma once

#include "game/ui/CountDownTimerControl.hh"
#include "game/ui/page/MenuPage.hh"

namespace UI {

class CharacterSelectPage : public MenuPage {
public:
    REPLACE void onBack(u32 localPlayerId);

private:
    void trySetAnim(u32 localPlayerId);
    void goBack();

    u8 _430[0x90C - 0x430];
    CountDownTimer *m_wifiCountDownTimer;
    LayoutUIControl *m_names;
    u8 _914[0x918 - 0x914];
};
static_assert(sizeof(CharacterSelectPage) == 0x918);

} // namespace UI
