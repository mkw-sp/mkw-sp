#pragma once

#include <Common.hh>

namespace Kart {

class KartMove {
public:
    f32 hardSpeedLimit() const;
    const Vec3 *internalVelDir() const;

private:
    u8 _000[0x020 - 0x000];
    f32 m_internalSpeed;
    u8 _024[0x02c - 0x024];
    f32 m_hardSpeedLimit;
    u8 _030[0x074 - 0x030];
    Vec3 m_internalVelDir;
    u8 _080[0x294 - 0x080];
};
static_assert(sizeof(KartMove) == 0x294);

} // namespace Kart
