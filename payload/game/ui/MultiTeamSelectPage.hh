#pragma once

#include "game/ui/MenuModelControl.hh"
#include "game/ui/Page.hh"
#include "game/ui/UpDownControl.hh"
#include "game/ui/ctrl/CtrlMenuBackButton.hh"
#include "game/ui/ctrl/CtrlMenuPageTitleText.hh"

namespace UI {

class MultiTeamSelectPage : public Page {
public:
    MultiTeamSelectPage();
    MultiTeamSelectPage(const MultiTeamSelectPage &) = delete;
    MultiTeamSelectPage(MultiTeamSelectPage &&) = delete;
    ~MultiTeamSelectPage() override;

    PageId getReplacement() override;
    void onInit() override;
    void onActivate() override;
    void beforeInAnim() override;
    void beforeOutAnim() override;
    void onRefocus() override;

private:
    void onBack(u32 localPlayerId);
    void onTeamControlFront(UpDownControl *control, u32 localPlayerId);
    void onTeamValueChange(TextUpDownValueControl::TextControl *text, u32 index);
    void onBackButtonFront(PushButton *button, u32 localPlayerId);

    template <typename T>
    using H = typename T::template Handler<MultiTeamSelectPage>;

    template <typename T>
    using C = typename T::template ChangeHandler<MultiTeamSelectPage>;

    MultiControlInputManager m_inputManager;
    CtrlMenuPageTitleText m_pageTitleText;
    DriverModelControl m_driverModels[4];
    LayoutUIControl m_nullControls[4];
    UpDownControl m_teamControls[4];
    TextUpDownValueControl m_teamValues[4];
    CtrlMenuBackButton m_backButton;
    H<MultiControlInputManager> m_onBack{this, &MultiTeamSelectPage::onBack};
    H<UpDownControl> m_onTeamControlFront{this, &MultiTeamSelectPage::onTeamControlFront};
    C<TextUpDownValueControl> m_onTeamValueChange{this, &MultiTeamSelectPage::onTeamValueChange};
    H<PushButton> m_onBackButtonFront{this, &MultiTeamSelectPage::onBackButtonFront};
    PageId m_replacement;
    std::optional<u8> m_teamCount;
};

} // namespace UI
