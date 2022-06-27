#pragma once

#include "game/ui/Page.hh"
#include "game/ui/ctrl/CtrlMenuBackButton.hh"
#include "game/ui/ctrl/CtrlMenuPageTitleText.hh"

namespace UI {

class ServicePackTopPage : public Page {
public:
    ServicePackTopPage();
    ServicePackTopPage(const ServicePackTopPage &) = delete;
    ServicePackTopPage(ServicePackTopPage &&) = delete;
    ~ServicePackTopPage() override;

    PageId getReplacement() override;
    void onInit() override;
    void onActivate() override;

private:
    void onBack(u32 localPlayerId);
    void onUpdateButtonFront(PushButton *button, u32 localPlayerId);
    void onChannelButtonFront(PushButton *button, u32 localPlayerId);
    void onBackButtonFront(PushButton *button, u32 localPlayerId);

    template <typename T>
    using H = typename T::Handler<ServicePackTopPage>;

    MultiControlInputManager m_inputManager;
    CtrlMenuPageTitleText m_pageTitleText;
    PushButton m_settingsButton;
    PushButton m_tracksButton;
    PushButton m_ghostsButton;
    PushButton m_updateButton;
    PushButton m_channelButton;
    PushButton m_aboutButton;
    CtrlMenuBackButton m_backButton;
    H<MultiControlInputManager> m_onBack{ this, &ServicePackTopPage::onBack };
    H<PushButton> m_onUpdateButtonFront{ this, &ServicePackTopPage::onUpdateButtonFront };
    H<PushButton> m_onChannelButtonFront{ this, &ServicePackTopPage::onChannelButtonFront };
    H<PushButton> m_onBackButtonFront{ this, &ServicePackTopPage::onBackButtonFront };
    PageId m_replacement;
};

} // namespace UI
