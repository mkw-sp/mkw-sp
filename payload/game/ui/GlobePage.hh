#pragma once

#include "game/ui/FriendMatchingPlayer.hh"
#include "game/ui/Page.hh"

namespace UI {

class GlobePage : public Page {
public:
    void REPLACED(afterCalc)();
    REPLACE virtual void afterCalc() override;

    void requestSpinLocal(int r4);
    REPLACE void requestSpinFriend(const System::Mii *mii, u32 friendIndex, void *callback);
    void requestComment(const System::Mii *mii, u16 latitude, u16 longitude, u32 location,
            u32 messageId, u32 type, const System::Mii *localPlayerMii,
            FriendMatchingPlayer::Callback *callback);
    void requestSpinFar();
    void requestSpinMid();
    void requestSpinClose();

    void showFriendInfo();

private:
    void handleSpinFriend();

    MenuInputManager m_inputManager;
    LayoutUIControl m_commentWindow;
    LayoutUIControl m_nameWindow;
    u32 m_state;
    System::Mii m_mii;
    u32 m_friendIndex;
    u8 _3fc[0x400 - 0x3fc];
    u32 m_messageId;
    u32 m_location;
    u8 _408[0x40c - 0x408];
    void *m_callback;
    u8 _4c8[0x4c8 - 0x410];
};

static_assert(sizeof(GlobePage) == 0x4c8);

} // namespace UI
