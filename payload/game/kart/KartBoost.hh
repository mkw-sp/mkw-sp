#pragma once

#include <Common.hh>

namespace Kart {

class KartBoost {
    friend class KartObjectProxy;
    friend class KartRollback;
    friend class KartSaveState;

public:
    KartBoost();
    virtual ~KartBoost() = delete;
    virtual void dt(s32 type);

private:
    s16 m_timesBeforeEnd[6];
    u16 m_types;
    f32 m_boostMultipler;
    f32 m_boostAcceleration;
    f32 m_1c;
    f32 m_boostSpeedLimit;
};

static_assert(sizeof(KartBoost) == 0x24);

} // namespace Kart
