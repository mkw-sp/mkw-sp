#pragma once

#include "game/ui/OnlineTeamSelectButton.hh"
#include "game/ui/Page.hh"
#include "game/ui/ctrl/CtrlMenuBackButton.hh"
#include "game/ui/ctrl/CtrlMenuInstructionText.hh"
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

    void onReceiveTeamSelect(u32 playerId, u32 teamId);

private:
    void onBack(u32 localPlayerId);
    void onButtonFront(PushButton *button, u32 localPlayerId);
    void onBackButtonFront(PushButton *button, u32 localPlayerId);

    template <typename T>
    using H = typename T::Handler<OnlineTeamSelectPage>;

    MultiControlInputManager m_inputManager;
    CtrlMenuPageTitleText m_pageTitleText;
    OnlineTeamSelectButton m_buttons[12];
    CtrlMenuBackButton m_backButton;
    CtrlMenuInstructionText m_instructionText;
    H<MultiControlInputManager> m_onBack{this, &OnlineTeamSelectPage::onBack};
    H<PushButton> m_onButtonFront{this, &OnlineTeamSelectPage::onButtonFront};
    H<PushButton> m_onBackButtonFront{this, &OnlineTeamSelectPage::onBackButtonFront};
    PageId m_replacement;
    MiiGroup m_miiGroup;
    u32 m_localPlayerCount;
};

} // namespace UI
