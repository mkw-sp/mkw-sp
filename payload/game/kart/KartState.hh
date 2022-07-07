#pragma once

#include <Common.hh>

namespace Kart {

class KartState {
public:
    bool inCannon() const;

private:
    u8 _00[0x08 - 0x00];
    u32 : 27;
    bool m_inCannon : 1;
    u32 : 4;
    u8 _0c[0xc0 - 0x0c];
};
static_assert(sizeof(KartState) == 0xc0);

} // namespace Kart
