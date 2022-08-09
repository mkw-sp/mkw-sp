#pragma once

#include "game/ui/Page.hh"
#include "game/ui/ctrl/CtrlMenuBackButton.hh"
#include "game/ui/ctrl/CtrlMenuPageTitleText.hh"

namespace UI {

class OnlineTopPage : public Page {
public:
    OnlineTopPage();
    OnlineTopPage(const OnlineTopPage &) = delete;
    OnlineTopPage(OnlineTopPage &&) = delete;
    ~OnlineTopPage() override;

    PageId getReplacement() override;
    void onInit() override;
    void onActivate() override;

private:
    void onBack(u32 localPlayerId);
    void onSettingsButtonFront(PushButton *button, u32 localPlayerId);
    void onConnectButtonFront(PushButton *button, u32 localPlayerId);
    void onBackButtonFront(PushButton *button, u32 localPlayerId);

    template <typename T>
    using H = typename T::Handler<OnlineTopPage>;

    MultiControlInputManager m_inputManager;
    CtrlMenuPageTitleText m_pageTitleText;
    PushButton m_settingsButton;
    PushButton m_connectButton;
    CtrlMenuBackButton m_backButton;
    H<MultiControlInputManager> m_onBack{ this, &OnlineTopPage::onBack };
    H<PushButton> m_onSettingsButtonFront{ this, &OnlineTopPage::onSettingsButtonFront };
    H<PushButton> m_onConnectButtonFront{ this, &OnlineTopPage::onConnectButtonFront };
    H<PushButton> m_onBackButtonFront{ this, &OnlineTopPage::onBackButtonFront };
    PageId m_replacement;
};

} // namespace UI
