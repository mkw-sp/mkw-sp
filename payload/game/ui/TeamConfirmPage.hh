#pragma once

#include "game/ui/Page.hh"
#include "game/ui/TeamConfirmControl.hh"
#include "game/ui/ctrl/CtrlMenuBackButton.hh"
#include "game/ui/ctrl/CtrlMenuPageTitleText.hh"

namespace UI {

class TeamConfirmPage : public Page {
public:
    TeamConfirmPage();
    TeamConfirmPage(const TeamConfirmPage &) = delete;
    TeamConfirmPage(TeamConfirmPage &&) = delete;
    ~TeamConfirmPage() override;

    PageId getReplacement() override;
    void onInit() override;
    void onActivate() override;

    REPLACE void prepareOfflineSingle();

    REPLACE static TeamConfirmPage *Get(PageId id);

private:
    void onBack(u32 localPlayerId);
    void onSettingsButtonFront(PushButton *button, u32 localPlayerId);
    void onOkButtonFront(PushButton *button, u32 localPlayerId);
    void onBackButtonFront(PushButton *button, u32 localPlayerId);

    template <typename T>
    using H = typename T::template Handler<TeamConfirmPage>;

    MultiControlInputManager m_inputManager;
    CtrlMenuPageTitleText m_pageTitleText;
    PushButton m_settingsButton;
    TeamConfirmControl m_controls[12];
    PushButton m_okButton;
    CtrlMenuBackButton m_backButton;
    H<MultiControlInputManager> m_onBack{this, &TeamConfirmPage::onBack};
    H<PushButton> m_onSettingsButtonFront{this, &TeamConfirmPage::onSettingsButtonFront};
    H<PushButton> m_onOkButtonFront{this, &TeamConfirmPage::onOkButtonFront};
    H<PushButton> m_onBackButtonFront{this, &TeamConfirmPage::onBackButtonFront};
    PageId m_replacement;
};

} // namespace UI
