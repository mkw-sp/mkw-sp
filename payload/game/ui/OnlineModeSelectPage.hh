#pragma once

#include "game/ui/Button.hh"
#include "game/ui/Page.hh"
#include "game/ui/ctrl/CtrlMenuBackButton.hh"
#include "game/ui/ctrl/CtrlMenuInstructionText.hh"
#include "game/ui/ctrl/CtrlMenuPageTitleText.hh"

namespace UI {

class OnlineModeSelectPage : public Page {
public:
    OnlineModeSelectPage() = default;
    ~OnlineModeSelectPage() override = default;

    void setRatings(u16 vsRating, u16 battleRating);

private:
    void onInit() override;
    void onActivate() override;
    void onBack(u32 localPlayerId);
    void onButtonFront(PushButton *button, u32 localPlayerId);
    void onButtonSelect(PushButton *button, u32 localPlayerId);
    void onBackButtonFront(PushButton *button, u32 localPlayerId);
    PageId getReplacement() override;

    template <typename T>
    using H = typename T::Handler<OnlineModeSelectPage>;

    H<MultiControlInputManager> m_onBack{this, &OnlineModeSelectPage::onBack};
    H<PushButton> m_onButtonFront{this, &OnlineModeSelectPage::onButtonFront};
    H<PushButton> m_onButtonSelect{this, &OnlineModeSelectPage::onButtonSelect};
    H<PushButton> m_onBackFront{this, &OnlineModeSelectPage::onBackButtonFront};

    CtrlMenuPageTitleText m_titleText;
    PushButton m_raceButton;
    PushButton m_battleButton;
    CtrlMenuBackButton m_backButton;
    CtrlMenuInstructionText m_instructionText;

    MultiControlInputManager m_inputManager;
    PageId m_replacement;
};

} // namespace UI
