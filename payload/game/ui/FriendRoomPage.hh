#pragma once

#include "game/ui/Page.hh"
#include "game/ui/MenuInputManager.hh"
#include "game/ui/SettingsPage.hh"
#include "game/ui/YesNoPage.hh"
#include "game/ui/ctrl/CtrlMenuBackButton.hh"
#include "game/ui/ctrl/CtrlMenuInstructionText.hh"

namespace UI {

class FriendRoomPage : public Page {
public:
    enum class RoomRole {
        None = -1,

        Host = 0,
        Player = 1,
        // We can almost certainly use this to join the room as a spectator
        // Spectator = 2,
    };

    FriendRoomPage();
    ~FriendRoomPage() override;

    void onInit() override;
    void onActivate() override;
    void onDeactivate() override;

private:
    void onBack(u32 localPlayerId);
    void onSettingsButtonFront(PushButton *button, u32 localPlayerId);
    void onCommentButtonFront(PushButton *button, u32 localPlayerId);
    void onRulesButtonFront(PushButton *button, u32 localPlayerId);
    void onCloseButtonFront(PushButton *button, u32 localPlayerId);
    void onRegisterButtonFront(PushButton *button, u32 localPlayerId);
    void onBackButtonFront(PushButton *button, u32 localPlayerId);
    void onButtonSelect(PushButton *button, u32 localPlayerId);
    void onBackConfirm(s32 choice, PushButton *button);
    void onSettingsBack(SettingsPage *settingsPage, PushButton *button);

    template <typename T>
    using H = typename T::Handler<FriendRoomPage>;

    MultiControlInputManager m_inputManager;
    PushButton m_settingsButton;
    PushButton m_commentButton;
    PushButton m_rulesButton;
    PushButton m_closeButton;
    PushButton m_registerButton;
    CtrlMenuBackButton m_backButton;
    CtrlMenuInstructionText m_instructionText;
    RoomRole m_roomRole;

    H<MultiControlInputManager> m_onBack{ this, &FriendRoomPage::onBack };
    H<PushButton> m_onSettingsButtonFront{ this, &FriendRoomPage::onSettingsButtonFront };
    H<PushButton> m_onCommentButtonFront{ this, &FriendRoomPage::onCommentButtonFront };
    H<PushButton> m_onRulesButtonFront{ this, &FriendRoomPage::onRulesButtonFront };
    H<PushButton> m_onCloseButtonFront{ this, &FriendRoomPage::onCloseButtonFront };
    H<PushButton> m_onRegisterButtonFront{ this, &FriendRoomPage::onRegisterButtonFront };
    H<PushButton> m_onBackButtonFront{ this, &FriendRoomPage::onBackButtonFront };
    H<PushButton> m_onButtonSelect{ this, &FriendRoomPage::onButtonSelect };
    H<YesNoPage> m_onBackConfirm{ this, &FriendRoomPage::onBackConfirm };
    H<SettingsPage> m_onSettingsBack{ this, &FriendRoomPage::onSettingsBack };
};

} // namespace UI
