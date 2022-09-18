#pragma once

#include "game/ui/Page.hh"
#include "game/ui/UpDownControl.hh"
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
    void onValueChange(TextUpDownValueControl::TextControl *text, u32 index);
    void onBackButtonFront(PushButton *button, u32 localPlayerId);

    template <typename T>
    using H = typename T::Handler<OnlineTeamSelectPage>;

    template <typename T>
    using C = typename T::ChangeHandler<OnlineTeamSelectPage>;

    MultiControlInputManager m_inputManager;
    CtrlMenuPageTitleText m_pageTitleText;
    UpDownControl m_controls[12];
    TextUpDownValueControl m_values[12];
    CtrlMenuBackButton m_backButton;
    H<MultiControlInputManager> m_onBack{ this, &OnlineTeamSelectPage::onBack };
    C<TextUpDownValueControl> m_onValueChange{ this, &OnlineTeamSelectPage::onValueChange };
    H<PushButton> m_onBackButtonFront{ this, &OnlineTeamSelectPage::onBackButtonFront };
    PageId m_replacement;
    u32 m_localPlayerCount;
};

} // namespace UI
