#pragma once

#include "game/ui/Page.hh"
#include "game/ui/MenuInputManager.hh"
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
    void afterCalc() override;
    void onRefocus() override;
private:
    void onMessageButtonFront(PushButton *button, u32 localPlayerId);
    void onCloseButtonFront(PushButton *button, u32 localPlayerId);
    void onRegisterButtonFront(PushButton *button, u32 localPlayerId);
    void onBackButtonFront(PushButton *button, u32 localPlayerId);
    void onButtonSelect(PushButton *button, u32 localPlayerId);
    void onBack(u32 localPlayerId);

    template <typename T>
    using H = typename T::Handler<FriendRoomPage>;

    MultiControlInputManager m_inputManager;
    PushButton m_messageButton;
    PushButton m_closeButton;
    PushButton m_registerButton;
    CtrlMenuBackButton m_backButton;
    // TODO: are message IDs hex?
    s32 m_messageId = 2001;
    CtrlMenuInstructionText m_instructionText;
    RoomRole m_roomRole;
    u32 m_maybePageState;
    bool m_somethingRegister;

    H<MultiControlInputManager> m_onBack{ this, &FriendRoomPage::onBack };
    H<PushButton> m_onMessageButtonFront{ this, &FriendRoomPage::onMessageButtonFront };
    H<PushButton> m_onCloseButtonFront{ this, &FriendRoomPage::onCloseButtonFront };
    H<PushButton> m_onRegisterButtonFront{ this, &FriendRoomPage::onRegisterButtonFront };
    H<PushButton> m_onBackButtonFront{ this, &FriendRoomPage::onBackButtonFront };
    H<PushButton> m_onButtonSelect{ this, &FriendRoomPage::onButtonSelect };
};

} // namespace UI
