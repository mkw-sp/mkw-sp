#include "FriendButton.hh"

namespace UI {

// Moved logic to OnlineFriendListPage::onFriendSelect
void FriendListButton::calc() {}

void FriendListButton::refresh(State state, u32 friendIndex) {
    m_state = state;
    m_friendIndex = friendIndex;
    m_animator.getGroup(4)->setAnimation(0, 0.0);

    if (state == State::Hidden) {
        setVisible(false);
        setPlayerFlags(0);
        return;
    }

    setVisible(true);
    setPlayerFlags(1);

    auto pane = m_mainLayout.findPaneByName("w_null");
    if (state == State::OneSided) {
        setPicture("chara", "no_linkmii");
        setPicture("chara_shadow", "no_linkmii");
        setPicture("chara_c_down", "no_linkmii");
        setPicture("chara_light_01", "no_linkmii");
        setPicture("chara_light_02", "no_linkmii");

        pane->flags = pane->flags & 0xfe;
    } else if (state == State::FullFriends) {
        setMiiPicture("chara", m_miiGroup, friendIndex, 0);
        setMiiPicture("chara_shadow", m_miiGroup, friendIndex, 0);
        setMiiPicture("chara_c_down", m_miiGroup, friendIndex, 0);
        setMiiPicture("chara_light_01", m_miiGroup, friendIndex, 0);
        setMiiPicture("chara_light_02", m_miiGroup, friendIndex, 0);

        pane->flags = (pane->flags & 0xfe) | 1;
    }
}

} // namespace UI
