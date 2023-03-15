#pragma once

#include "game/ui/FriendMatchingPlayer.hh"
#include "game/ui/Page.hh"

namespace UI {

class GlobePage : public Page {
public:
    void requestComment(const System::Mii *mii, u16 latitude, u16 longitude, u32 location,
            u32 messageId, u32 type, const System::Mii *localPlayerMii,
            FriendMatchingPlayer::Callback &callback);
    void requestSpinFar();
    void requestSpinMid();
    void requestSpinClose();

private:
    u8 _044[0x4c8 - 0x044];
};
static_assert(sizeof(GlobePage) == 0x4c8);

} // namespace UI
