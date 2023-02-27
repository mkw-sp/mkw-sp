#pragma once

#include "game/ui/Page.hh"
#include "game/ui/YesNoPage.hh"
#include "game/ui/FriendButton.hh"
#include "game/ui/ctrl/CtrlMenuBackButton.hh"
#include "game/ui/ctrl/CtrlMenuPageTitleText.hh"
#include "game/ui/ctrl/CtrlMenuInstructionText.hh"

namespace UI {

class OnlineTopPage : public Page {
public:
    OnlineTopPage() = default;
    ~OnlineTopPage() override = default;

    PageId getReplacement() override;
    void onInit() override;
    void onActivate() override;
private:
    void showUnimplemented();

    void onBack(u32 localPlayerId);
    void onButtonSelect(PushButton *button, u32 localPlayerId);
    void onWorldwideButtonFront(PushButton *button, u32 localPlayerId);
    void onTrackpackButtonFront(PushButton *button, u32 localPlayerId);
    void onFriendButtonFront(PushButton *button, u32 localPlayerId);
    void onDirectButtonFront(PushButton *button, u32 localPlayerId);
    void onBackButtonFront(PushButton *button, u32 localPlayerId);

    void onDirectConfirm(s32 choice, PushButton *button);

    template <typename T>
    using H = typename T::Handler<OnlineTopPage>;

    MultiControlInputManager m_inputManager;
    CtrlMenuPageTitleText m_pageTitleText;
    PushButton m_worldwideButton;
    PushButton m_trackpackButton;
    FriendButton m_friendButton;
    PushButton m_directButton;
    CtrlMenuBackButton m_backButton;
    CtrlMenuInstructionText m_instructionText;

    H<MultiControlInputManager> m_onBack{ this, &OnlineTopPage::onBack };
    H<PushButton> m_onButtonSelect{ this, &OnlineTopPage::onButtonSelect };
    H<PushButton> m_onWorldWideButtonFront{ this, &OnlineTopPage::onWorldwideButtonFront };
    H<PushButton> m_onTrackpackButtonFront{ this, &OnlineTopPage::onTrackpackButtonFront };
    H<PushButton> m_onFriendButtonFront{ this, &OnlineTopPage::onFriendButtonFront };
    H<PushButton> m_onDirectButtonFront{ this, &OnlineTopPage::onDirectButtonFront };
    H<PushButton> m_onBackButtonFront{ this, &OnlineTopPage::onBackButtonFront };

    H<YesNoPage> m_onDirectConfirm{ this, &OnlineTopPage::onDirectConfirm };

    PageId m_replacement;
};

} // namespace UI
