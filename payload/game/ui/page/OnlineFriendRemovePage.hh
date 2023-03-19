#pragma once

#include "game/ui/Button.hh"
#include "game/ui/Page.hh"
#include "game/ui/ctrl/CtrlMenuBackButton.hh"
#include "game/ui/ctrl/CtrlMenuPageTitleText.hh"

namespace UI {

class OnlineFriendRemovePage : public Page {
public:
    OnlineFriendRemovePage();
    ~OnlineFriendRemovePage() override;

    REPLACE void onRefocus() override;

private:
    REPLACE void onYesButtonFront(PushButton *button, u32 localPlayerId);

    u8 _44[0x80 - 0x44]; // Input handlers
    MultiControlInputManager m_inputManager;

    CtrlMenuPageTitleText m_pageTitleText;
    PushButton m_yesButton;
    PushButton m_noButton;
    CtrlMenuBackButton m_backButton;

    PageId m_replacement;
};

static_assert(sizeof(OnlineFriendRemovePage) == 0xb28);

} // namespace UI
