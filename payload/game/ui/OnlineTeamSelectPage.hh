#pragma once

#include "game/ui/Page.hh"
#include "game/ui/TeamBoxControl.hh"
#include "game/ui/ctrl/CtrlMenuBackButton.hh"
#include "game/ui/ctrl/CtrlMenuPageTitleText.hh"

namespace UI {

class OnlineTeamSelectPage : public Page {
public:
    OnlineTeamSelectPage();
    OnlineTeamSelectPage(const OnlineTeamSelectPage &) = delete;
    OnlineTeamSelectPage(OnlineTeamSelectPage &&) = delete;
    ~OnlineTeamSelectPage() override;

    PageId getReplacement() override;
    void onInit() override;
    void onActivate() override;

private:
    void onBack(u32 localPlayerId);
    void onBackButtonFront(PushButton *button, u32 localPlayerId);

    template <typename T>
    using H = typename T::Handler<OnlineTeamSelectPage>;

    template <typename T>
    using C = typename T::ChangeHandler<OnlineTeamSelectPage>;

    MultiControlInputManager m_inputManager;
    CtrlMenuPageTitleText m_pageTitleText;
    TeamBoxControl m_teamBoxControls[6];
    CtrlMenuBackButton m_backButton;
    H<MultiControlInputManager> m_onBack{ this, &OnlineTeamSelectPage::onBack };
    H<PushButton> m_onBackButtonFront{ this, &OnlineTeamSelectPage::onBackButtonFront };
    PageId m_replacement;
    u32 m_localPlayerCount;
};

} // namespace UI
