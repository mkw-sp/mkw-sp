#pragma once

#include <Common.hh>

namespace Kart {

class KartState {
    friend class KartMove;
    friend class KartRollback;
    friend class KartSaveState;
    friend class KartSub;

public:
    bool inCannon() const;

private:
    u8 _00[0x04 - 0x00];
    u32 : 27;
    bool m_beforeRespawn : 1;
    u32 : 4;
    u32 : 23;
    bool m_blinking : 1;
    u32 : 3;
    bool m_inCannon : 1;
    u32 : 4;
    u8 _0c[0x14 - 0x0c];
    u32 : 31;
    bool m_isCpu : 1;
    u8 _18[0x1c - 0x18];
    u32 m_airtime;
    u8 _20[0xc0 - 0x20];
};
static_assert(sizeof(KartState) == 0xc0);

} // namespace Kart
