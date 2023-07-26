#pragma once

#include "game/ui/Page.hh"
#include "game/ui/ctrl/CtrlMenuBackButton.hh"
#include "game/ui/ctrl/CtrlMenuInstructionText.hh"
#include "game/ui/ctrl/CtrlMenuPageTitleText.hh"

namespace UI {

class MultiTopPage : public Page {
public:
    MultiTopPage();
    MultiTopPage(const MultiTopPage &) = delete;
    MultiTopPage(MultiTopPage &&) = delete;
    ~MultiTopPage() override;

    PageId getReplacement() override;
    void onInit() override;
    void onActivate() override;

private:
    void onBack(u32 localPlayerId);
    void onSettingsButtonFront(PushButton *button, u32 localPlayerId);
    void onSettingsButtonSelect(PushButton *button, u32 localPlayerId);
    void onVSButtonFront(PushButton *button, u32 localPlayerId);
    void onVSButtonSelect(PushButton *button, u32 localPlayerId);
    void onBTButtonFront(PushButton *button, u32 localPlayerId);
    void onBTButtonSelect(PushButton *button, u32 localPlayerId);
    void onBackButtonFront(PushButton *button, u32 localPlayerId);
    void onBackButtonSelect(PushButton *button, u32 localPlayerId);
    void onBackCommon(f32 delay);

    template <typename T>
    using H = typename T::template Handler<MultiTopPage>;

    MultiControlInputManager m_inputManager;
    CtrlMenuPageTitleText m_pageTitleText;
    PushButton m_settingsButton;
    PushButton m_vsButton;
    PushButton m_btButton;
    CtrlMenuInstructionText m_instructionText;
    CtrlMenuBackButton m_backButton;
    H<MultiControlInputManager> m_onBack{this, &MultiTopPage::onBack};
    H<PushButton> m_onSettingsButtonFront{this, &MultiTopPage::onSettingsButtonFront};
    H<PushButton> m_onSettingsButtonSelect{this, &MultiTopPage::onSettingsButtonSelect};
    H<PushButton> m_onVSButtonFront{this, &MultiTopPage::onVSButtonFront};
    H<PushButton> m_onVSButtonSelect{this, &MultiTopPage::onVSButtonSelect};
    H<PushButton> m_onBTButtonFront{this, &MultiTopPage::onBTButtonFront};
    H<PushButton> m_onBTButtonSelect{this, &MultiTopPage::onBTButtonSelect};
    H<PushButton> m_onBackButtonFront{this, &MultiTopPage::onBackButtonFront};
    H<PushButton> m_onBackButtonSelect{this, &MultiTopPage::onBackButtonSelect};
    PageId m_replacement;
    bool m_reset;
};

} // namespace UI
