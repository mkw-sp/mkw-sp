#pragma once

#include "game/ui/MessagePage.hh"
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
    void onBackspaceButtonFront(PushButton *button, u32 localPlayerId);
    void onResetButtonFront(PushButton *button, u32 localPlayerId);
    void onOkButtonFront(PushButton *button, u32 localPlayerId);
    void onBackButtonFront(PushButton *button, u32 localPlayerId);
    void onBadConnectCode(MessagePage *messagePage);

    template <typename T>
    using H = typename T::Handler<DirectConnectionPage>;

    MultiControlInputManager m_inputManager;
    CtrlMenuPageTitleText m_pageTitleText;
    NumericEditBox m_editBox;
    PushButton m_digitButtons[10];
    PushButton m_backspaceButton;
    PushButton m_resetButton;
    PushButton m_okButton;
    CtrlMenuBackButton m_backButton;
    H<MultiControlInputManager> m_onBack{ this, &DirectConnectionPage::onBack };
    H<PushButton> m_onDigitButtonFront{ this, &DirectConnectionPage::onDigitButtonFront };
    H<PushButton> m_onBackspaceButtonFront{ this, &DirectConnectionPage::onBackspaceButtonFront };
    H<PushButton> m_onResetButtonFront{ this, &DirectConnectionPage::onResetButtonFront };
    H<PushButton> m_onOkButtonFront{ this, &DirectConnectionPage::onOkButtonFront };
    H<PushButton> m_onBackButtonFront{ this, &DirectConnectionPage::onBackButtonFront };
    H<MessagePage> m_onBadConnectCode{ this, &DirectConnectionPage::onBadConnectCode };
    PageId m_replacement;
    u32 m_ip;
    u16 m_port;
    u16 m_passcode;
};

} // namespace UI
