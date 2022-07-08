#pragma once

#include <Common.hh>

namespace Sound {

class ItemMusicManager {
private:
    enum class State {
        KinokoBig = 0,
        Star = 1,
        Damage = 2,
        Idle = 3,
    };

    void REPLACED(resolve)();
    REPLACE void resolve();

    u8 _00[0x10 - 0x00];
    State m_state;
    u8 _14[0x2c - 0x14];
};
static_assert(sizeof(ItemMusicManager) == 0x2c);

} // namespace Sound
