#pragma once

#include "game/ui/Page.hh"
#include "game/ui/ctrl/CtrlMenuBackButton.hh"
#include "game/ui/ctrl/CtrlMenuInstructionText.hh"
#include "game/ui/ctrl/CtrlMenuPageTitleText.hh"

#include <features/mission_mode/MissionMode.hh>

namespace UI {

class SingleTopPage : public Page {
public:
    SingleTopPage();
    SingleTopPage(const SingleTopPage &) = delete;
    SingleTopPage(SingleTopPage &&) = delete;
    ~SingleTopPage() override;

    PageId getReplacement() override;
    void onInit() override;
    void onActivate() override;

private:
    void onBack(u32 localPlayerId);
    void onSettingsButtonFront(PushButton *button, u32 localPlayerId);
    void onSettingsButtonSelect(PushButton *button, u32 localPlayerId);
    void onTAButtonFront(PushButton *button, u32 localPlayerId);
    void onTAButtonSelect(PushButton *button, u32 localPlayerId);
    void onVSButtonFront(PushButton *button, u32 localPlayerId);
    void onVSButtonSelect(PushButton *button, u32 localPlayerId);
    void onBTButtonFront(PushButton *button, u32 localPlayerId);
    void onBTButtonSelect(PushButton *button, u32 localPlayerId);
#if ENABLE_MISSION_MODE
    void onMRButtonFront(PushButton *button, u32 localPlayerId);
    void onMRButtonSelect(PushButton *button, u32 localPlayerId);
#endif
    void onBackButtonFront(PushButton *button, u32 localPlayerId);
    void onBackButtonSelect(PushButton *button, u32 localPlayerId);
    void onBackCommon(f32 delay);

    template <typename T>
    using H = typename T::template Handler<SingleTopPage>;

    MultiControlInputManager m_inputManager;
    CtrlMenuPageTitleText m_pageTitleText;
    PushButton m_settingsButton;
    PushButton m_taButton;
    PushButton m_vsButton;
    PushButton m_btButton;
#if ENABLE_MISSION_MODE
    PushButton m_mrButton;
#endif
    CtrlMenuInstructionText m_instructionText;
    CtrlMenuBackButton m_backButton;
    H<MultiControlInputManager> m_onBack{this, &SingleTopPage::onBack};
    H<PushButton> m_onSettingsButtonFront{this, &SingleTopPage::onSettingsButtonFront};
    H<PushButton> m_onSettingsButtonSelect{this, &SingleTopPage::onSettingsButtonSelect};
    H<PushButton> m_onTAButtonFront{this, &SingleTopPage::onTAButtonFront};
    H<PushButton> m_onTAButtonSelect{this, &SingleTopPage::onTAButtonSelect};
    H<PushButton> m_onVSButtonFront{this, &SingleTopPage::onVSButtonFront};
    H<PushButton> m_onVSButtonSelect{this, &SingleTopPage::onVSButtonSelect};
    H<PushButton> m_onBTButtonFront{this, &SingleTopPage::onBTButtonFront};
    H<PushButton> m_onBTButtonSelect{this, &SingleTopPage::onBTButtonSelect};
#if ENABLE_MISSION_MODE
    H<PushButton> m_onMRButtonFront{this, &SingleTopPage::onMRButtonFront};
    H<PushButton> m_onMRButtonSelect{this, &SingleTopPage::onMRButtonSelect};
#endif
    H<PushButton> m_onBackButtonFront{this, &SingleTopPage::onBackButtonFront};
    H<PushButton> m_onBackButtonSelect{this, &SingleTopPage::onBackButtonSelect};
    PageId m_replacement;
};

} // namespace UI
