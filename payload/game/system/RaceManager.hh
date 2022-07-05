#pragma once

#include "game/system/InputManager.hh"

namespace System {

class RaceManager {
public:
    class Player {
    public:
        PadProxy *padProxy();

    private:
        u8 _00[0x48 - 0x00];
        PadProxy *m_padProxy;
        u8 _4c[0x54 - 0x4c];
    };
    static_assert(sizeof(Player) == 0x54);

    Player *player(u32 playerId);

    static RaceManager *Instance();

private:
    u8 _00[0x0c - 0x00];
    Player **m_players;
    u8 _10[0x50 - 0x10];

    static RaceManager *s_instance;
};
static_assert(sizeof(RaceManager) == 0x50);

} // namespace System
