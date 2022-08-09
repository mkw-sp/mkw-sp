#include "FriendMatchingPlayer.hh"

namespace UI {

FriendMatchingPlayer::~FriendMatchingPlayer() = default;

FriendMatchingPlayer::Callback &FriendMatchingPlayer::callback() {
    return m_callback;
}

} // namespace UI
