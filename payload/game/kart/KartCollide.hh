#pragma once

#include <Common.hh>

namespace Kart {

class KartCollide {
    friend class KartRollback;

private:
    u8 _00[0x3c - 0x00];
    Vec3 m_movement;
    u8 _48[0x74 - 0x48];
};
static_assert(sizeof(KartCollide) == 0x74);

} // namespace Kart
