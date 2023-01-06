#pragma once

#include <Common.hh>

namespace Kart {

class KartBoost {
    friend class KartObjectProxy;
    friend class KartRollback;

private:
    u8 _00[0x04 - 0x00];
    s16 m_timesBeforeEnd[6];
    u16 m_types;
    u8 _12[0x24 - 0x12];
};
static_assert(sizeof(KartBoost) == 0x24);

} // namespace Kart
