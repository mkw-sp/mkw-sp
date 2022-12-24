#pragma once

#include <Common.hh>

namespace Kart {

class VehiclePhysics {
public:
    const Vec3 *externalVel() const;
    const Vec3 *movingRoadVel() const;
    const Vec3 *movingWaterVel() const;

private:
    u8 _000[0x074 - 0x000];
    Vec3 m_externalVel;
    u8 _080[0x0b0 - 0x080];
    Vec3 m_movingRoadVel;
    u8 _0bc[0x0c8 - 0x0bc];
    Vec3 m_movingWaterVel;
    u8 _0d4[0x1b4 - 0x0d4];
};
static_assert(sizeof(VehiclePhysics) == 0x1b4);

} // namespace Kart
