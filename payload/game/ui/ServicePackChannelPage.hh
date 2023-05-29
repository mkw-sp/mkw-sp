#pragma once

#include "game/ui/Page.hh"

#include "game/ui/ctrl/CtrlMenuBackButton.hh"
#include "game/ui/ctrl/CtrlMenuPageTitleText.hh"

namespace UI {

class ServicePackChannelPage : public Page {
public:
    ServicePackChannelPage();
    ~ServicePackChannelPage() override;

    void onInit() override;

private:
    void onBack(u32 localPlayerId);
    void onRankingsButtonFront(PushButton *pushButton, u32 localPlayerId);
    void onBackButtonFront(PushButton *pushButton, u32 localPlayerId);

    template <typename T>
    using H = typename T::template Handler<ServicePackChannelPage>;

    MultiControlInputManager m_inputManager;
    PushButton m_rankingsButton;
    CtrlMenuPageTitleText m_pageTitleText;
    CtrlMenuBackButton m_backButton;
    H<MultiControlInputManager> m_onBack{this, &ServicePackChannelPage::onBack};
    H<PushButton> m_onRankingsButtonFront{this, &ServicePackChannelPage::onRankingsButtonFront};
    H<PushButton> m_onBackButtonFront{this, &ServicePackChannelPage::onBackButtonFront};
};

} // namespace UI
