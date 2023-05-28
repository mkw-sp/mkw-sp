#pragma once

#include <common/TQuat.hh>
#include <common/TVec3.hh>

namespace Kart {

class VehiclePhysics {
    friend class KartObjectProxy;
    friend class KartRollback;
    friend class KartSaveState;

public:
    const Vec3 *externalVel() const;
    const Vec3 *internalVel() const;
    const Vec3 *movingRoadVel() const;
    const Vec3 *movingWaterVel() const;

private:
    u8 _000[0x068 - 0x000];
    Vec3 m_pos;
    Vec3 m_externalVel;
    u8 _080[0x0b0 - 0x080];
    Vec3 m_movingRoadVel;
    u8 _0bc[0x0c8 - 0x0bc];
    Vec3 m_movingWaterVel;
    u8 _0d4[0x0f0 - 0x0d4];
    Quat m_mainRot;
    u8 _100[0x14c - 0x100];
    Vec3 m_internalVel;
    u8 _158[0x174 - 0x158];
    bool m_inBullet;
    u8 _175[0x1b4 - 0x175];
};
static_assert(sizeof(VehiclePhysics) == 0x1b4);

} // namespace Kart
