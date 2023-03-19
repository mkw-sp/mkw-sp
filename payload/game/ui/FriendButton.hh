#pragma once

#include "game/ui/Button.hh"

namespace UI {

class FriendButton : public PushButton {
public:
    void load(const char **animInfo, const char *dir, const char *file, const char *variant,
            bool r8, bool pointerOnly);

    MiiGroup *m_miiGroup;
    u32 m_friendIndex;
};

class FriendListButton : public FriendButton {
public:
    FriendListButton();
    ~FriendListButton() override;
    REPLACE virtual void calc() override;

    enum class State {
        Hidden,
        OneSided,
        FullFriends,
    };

    void refresh(State state, u32 friendIndex);
    State m_state;

private:
    BOOL m_isSelected; // Unused due to replaced calc
};

static_assert(sizeof(FriendButton) == 0x25c);
static_assert(sizeof(FriendListButton) == 0x264);

} // namespace UI
