#pragma once

#include "game/system/InputManager.hh"

namespace System {

class RaceManager {
public:
    class Player {
    public:
        u8 rank() const;
        u8 maxLap() const;
        bool hasFinished() const;
        PadProxy *padProxy();

    private:
        u8 _00[0x20 - 0x00];
        u8 m_rank;
        u8 _21[0x26 - 0x21];
        u8 m_maxLap;
        u8 _27[0x38 - 0x27];
        u32 _pad0 : 30;
        bool m_hasFinished : 1;
        u32 _pad1 : 1;
        u8 _3c[0x48 - 0x3c];
        PadProxy *m_padProxy;
        u8 _4c[0x54 - 0x4c];
    };
    static_assert(sizeof(Player) == 0x54);

    Player *player(u32 playerId);
    void REPLACED(calc)();
    REPLACE void calc();

    static REPLACE RaceManager *CreateInstance();
    static RaceManager *Instance();
    static u8 GetLapCount();

private:
    RaceManager();

    u8 _00[0x0c - 0x00];
    Player **m_players;
    u8 _10[0x20 - 0x10];
    u32 m_frame;
    u8 _24[0x2d - 0x24];
    bool m_spectatorMode;
    bool m_canStartCountdown;
    u8 _2e[0x4c - 0x2f];

    static RaceManager *s_instance;
};
static_assert(sizeof(RaceManager) == 0x4c);

} // namespace System
