#pragma once

#include "game/battle/BalloonUnit.hh"

namespace Battle {

class BalloonManager {
public:
    void REPLACED(add)(u32 playerId, u32 teamId, u32 r6, u32 r7, u32 count, u32 r9);
    REPLACE void add(u32 playerId, u32 teamId, u32 r6, u32 r7, u32 count, u32 r9);

    REPLACE static BalloonManager *CreateInstance();

private:
    BalloonManager();

    struct Balloon {
        BalloonUnit *unit;
        u8 teamId;
        u8 _5[0x8 - 0x5];
    };
    static_assert(sizeof(Balloon) == 0x8);

    struct Player {
        u8 _00[0x18 - 0x00];
    };
    static_assert(sizeof(Player) == 0x18);

    u8 m_playerCount;
    u8 m_balloonsPerTeam;
    u8 m_balloonCount;
    u8 _003[0x004 - 0x003];
    Balloon m_balloons[120];
    Player m_players[12];
    u8 _4e4[0x4f8 - 0x4e4];

    static BalloonManager *s_instance;
};
static_assert(sizeof(BalloonManager) == 0x4f8);

} // namespace Battle
