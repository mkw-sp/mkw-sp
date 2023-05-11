#pragma once

#include <Common.hh>

namespace Kart {

class KartBoost {
    friend class KartObjectProxy;
    friend class KartRollback;

public:
    KartBoost();
    virtual ~KartBoost();

private:
    s16 m_timesBeforeEnd[6];
    u16 m_types;
    u8 _12[0x24 - 0x12];
};
static_assert(sizeof(KartBoost) == 0x24);

} // namespace Kart
