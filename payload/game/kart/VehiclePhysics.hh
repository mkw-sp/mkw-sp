#pragma once

#include <common/Vec3.hh>

namespace Kart {

class VehiclePhysics {
    friend class KartObjectProxy;
    friend class KartRollback;

public:
    const Vec3<f32> *externalVel() const;
    const Vec3<f32> *internalVel() const;
    const Vec3<f32> *movingRoadVel() const;
    const Vec3<f32> *movingWaterVel() const;

private:
    u8 _000[0x068 - 0x000];
    Vec3<f32> m_pos;
    Vec3<f32> m_externalVel;
    u8 _080[0x0b0 - 0x080];
    Vec3<f32> m_movingRoadVel;
    u8 _0bc[0x0c8 - 0x0bc];
    Vec3<f32> m_movingWaterVel;
    u8 _0d4[0x0f0 - 0x0d4];
    Quat m_mainRot;
    u8 _100[0x14c - 0x100];
    Vec3<f32> m_internalVel;
    u8 _158[0x1b4 - 0x158];
};
static_assert(sizeof(VehiclePhysics) == 0x1b4);

} // namespace Kart
