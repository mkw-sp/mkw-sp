#pragma once

#include "game/ui/FriendButton.hh"
#include "game/ui/Page.hh"
#include "game/ui/ctrl/CtrlMenuBackButton.hh"
#include "game/ui/ctrl/CtrlMenuInstructionText.hh"
#include "game/ui/ctrl/CtrlMenuPageTitleText.hh"

namespace UI {

class OnlineFriendMenuPage : public Page {
public:
    OnlineFriendMenuPage() = default;
    ~OnlineFriendMenuPage() = default;

    PageId getReplacement() override;
    void onInit() override;
    void onActivate() override;
    void onDeactivate() override;

private:
    void onFriendListFront(PushButton *button, u32 localPlayerId);
    void onRegisterFront(PushButton *button, u32 localPlayerId);
    void onStartRoomFront(PushButton *button, u32 localPlayerId);
    void onButtonSelect(PushButton *button, u32 localPlayerId);
    void onBackFront(PushButton *button, u32 localPlayerId);
    void onBack(u32 localPlayerId);

    template <typename T>
    using H = typename T::Handler<OnlineFriendMenuPage>;

    CtrlMenuPageTitleText m_titleText;
    CtrlMenuInstructionText m_instructionText;
    FriendButton m_friendListButton;
    PushButton m_registerButton;
    PushButton m_startRoomButton;
    CtrlMenuBackButton m_backButton;

    H<MultiControlInputManager> m_onBack{this, &OnlineFriendMenuPage::onBack};
    H<PushButton> m_onButtonSelect{this, &OnlineFriendMenuPage::onButtonSelect};
    H<PushButton> m_onFriendListFront{this, &OnlineFriendMenuPage::onFriendListFront};
    H<PushButton> m_onRegisterFront{this, &OnlineFriendMenuPage::onRegisterFront};
    H<PushButton> m_onStartRoomFront{this, &OnlineFriendMenuPage::onStartRoomFront};
    H<PushButton> m_onBackFront{this, &OnlineFriendMenuPage::onBackFront};

    PageId m_replacement;
    MultiControlInputManager m_inputManager;
};

} // namespace UI
