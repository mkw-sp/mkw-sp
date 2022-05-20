#pragma once

extern "C" {
#include "ServicePackTopPage.h"
}

#include "game/ui/Page.hh"
#include "game/ui/ctrl/CtrlMenuBackButton.hh"
#include "game/ui/ctrl/CtrlMenuPageTitleText.hh"

namespace UI {

class ServicePackTopPage : public Page {
public:
    ServicePackTopPage();
    ~ServicePackTopPage() override;
    void onInit() override;

private:
    void onBack(u32 localPlayerId);
    void onBackButtonFront(PushButton *button, u32 UNUSED(localPlayerId));

    MultiControlInputManager m_inputManager;
    CtrlMenuPageTitleText m_pageTitleText;
    PushButton m_settingsButton;
    PushButton m_tracksButton;
    PushButton m_ghostsButton;
    PushButton m_updatesButton;
    PushButton m_channelButton;
    PushButton m_aboutButton;
    CtrlMenuBackButton m_backButton;
    MultiControlInputManager::InputHandler<ServicePackTopPage> m_onBack;
    PushButton::InputHandler<ServicePackTopPage> m_onBackButtonFront;
};

} // namespace UI
