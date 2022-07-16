#pragma once

#include "game/ui/Page.hh"
#include "game/ui/ctrl/CtrlMenuBackButton.hh"
#include "game/ui/ctrl/CtrlMenuPageTitleText.hh"

namespace UI {

class TimeAttackModeSelectPage : public Page {
public:
    TimeAttackModeSelectPage();
    TimeAttackModeSelectPage(const TimeAttackModeSelectPage &) = delete;
    TimeAttackModeSelectPage(TimeAttackModeSelectPage &&) = delete;
    ~TimeAttackModeSelectPage() override;

    PageId getReplacement() override;
    void onInit() override;
    void onActivate() override;
    void onRefocus() override;

private:
    void onBack(u32 localPlayerId);
    void onSettingsButtonFront(PushButton *button, u32 localPlayerId);
    void onButtonFront(PushButton *button, u32 localPlayerId);
    void onBackButtonFront(PushButton *button, u32 localPlayerId);

    template <typename T>
    using H = typename T::Handler<TimeAttackModeSelectPage>;

    MultiControlInputManager m_inputManager;
    CtrlMenuPageTitleText m_pageTitleText;
    PushButton m_settingsButton;
    PushButton m_buttons2[2];
    PushButton m_buttons3[3];
    CtrlMenuBackButton m_backButton;
    H<MultiControlInputManager> m_onBack{ this, &TimeAttackModeSelectPage::onBack };
    H<PushButton> m_onSettingsButtonFront{ this, &TimeAttackModeSelectPage::onSettingsButtonFront };
    H<PushButton> m_onButtonFront{ this, &TimeAttackModeSelectPage::onButtonFront };
    H<PushButton> m_onBackButtonFront{ this, &TimeAttackModeSelectPage::onBackButtonFront };
    bool m_isVanilla;
    bool m_isReplay;
    PageId m_replacement;
};

} // namespace UI
