#pragma once

#include "game/ui/NumericEditBox.hh"
#include "game/ui/Page.hh"
#include "game/ui/ctrl/CtrlMenuBackButton.hh"
#include "game/ui/ctrl/CtrlMenuPageTitleText.hh"

namespace UI {

class DirectConnectionPage : public Page {
public:
    DirectConnectionPage();
    DirectConnectionPage(const DirectConnectionPage &) = delete;
    DirectConnectionPage(DirectConnectionPage &&) = delete;
    ~DirectConnectionPage() override;

    PageId getReplacement() override;
    void onInit() override;
    void onActivate() override;

private:
    void onBack(u32 localPlayerId);
    void onDigitButtonFront(PushButton *button, u32 localPlayerId);
    void onBackButtonFront(PushButton *button, u32 localPlayerId);

    template <typename T>
    using H = typename T::Handler<DirectConnectionPage>;

    MultiControlInputManager m_inputManager;
    CtrlMenuPageTitleText m_pageTitleText;
    NumericEditBox m_editBox;
    PushButton m_digitButtons[10];
    PushButton m_backspaceButton;
    PushButton m_okButton;
    CtrlMenuBackButton m_backButton;
    H<MultiControlInputManager> m_onBack{ this, &DirectConnectionPage::onBack };
    H<PushButton> m_onDigitButtonFront{ this, &DirectConnectionPage::onDigitButtonFront };
    H<PushButton> m_onBackButtonFront{ this, &DirectConnectionPage::onBackButtonFront };
    PageId m_replacement;
};

} // namespace UI
