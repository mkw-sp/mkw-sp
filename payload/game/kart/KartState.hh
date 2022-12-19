#pragma once

#include <Common.hh>

namespace Kart {

class KartState {
    friend class KartMove;
    friend class KartSub;

public:
    bool inCannon() const;

private:
    u8 _00[0x08 - 0x00];
    u32 : 23;
    bool m_blinking : 1;
    u32 : 3;
    bool m_inCannon : 1;
    u32 : 4;
    u8 _0c[0x14 - 0x0c];
    u32 : 31;
    bool m_isCpu : 1;
    u8 _18[0xc0 - 0x18];
};
static_assert(sizeof(KartState) == 0xc0);

} // namespace Kart
