#pragma once

#include "game/ui/Page.hh"
#include "game/ui/Button.hh"
#include "game/ui/ctrl/CtrlMenuPageTitleText.hh"

namespace UI {

class OnlineModeSelectPage : public Page {
private:
    REPLACE void onActivate() override;
    REPLACE void onButtonFront(PushButton *button, u32 localPlayerId);

    u8 _44[0x94 - 0x44];
    CtrlMenuPageTitleText m_pageTitleText;
    u8 _208[0xcac - 0x208];
    PageId m_replacement;
};

static_assert(sizeof(OnlineModeSelectPage) == 0xcb0);

}
