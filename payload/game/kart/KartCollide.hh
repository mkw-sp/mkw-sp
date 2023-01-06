#pragma once

#include <common/TVec3.hh>

namespace Kart {

class KartCollide {
    friend class KartObjectProxy;
    friend class KartRollback;

private:
    u8 _00[0x3c - 0x00];
    Vec3 m_movement;
    s16 m_timeBeforeRespawn;
    u8 _4a[0x74 - 0x4a];
};
static_assert(sizeof(KartCollide) == 0x74);

} // namespace Kart
