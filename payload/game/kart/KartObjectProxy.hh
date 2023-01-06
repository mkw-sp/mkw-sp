#pragma once

#include <common/TQuat.hh>
#include <common/TVec3.hh>

namespace Kart {

class KartCollide;
class KartMove;
class KartRollback;
class KartState;
class VehiclePhysics;

struct KartAccessor {
    u8 _00[0x04 - 0x00];
    KartState *state;
    u8 _08[0x28 - 0x08];
    KartMove *move;
    u8 _2c[0x30 - 0x2c];
    KartCollide *collide;
    u8 _34[0x3c - 0x34];
    KartRollback *rollback; // Replaced
    u8 _40[0x64 - 0x40];
};
static_assert(sizeof(KartAccessor) == 0x64);

class KartObjectProxy {
public:
    KartObjectProxy();

    const Vec3 *getPos() const;
    const Vec3 *getLastPos() const;
    const Quat *getMainRot() const;
    KartState *getKartState();
    const VehiclePhysics *getVehiclePhysics() const;
    VehiclePhysics *getVehiclePhysics();
    bool isCPU() const;
    bool isGhost() const;
    KartMove *getKartMove();
    KartCollide *getKartCollide();
    const KartRollback *getKartRollback() const;
    u32 getPlayerId() const;
    f32 getInternalSpeed() const;
    s16 getTimeBeforeRespawn() const;
    u16 getTimeInRespawn() const;
    s16 getTimeBeforeBoostEnd(u8 type) const;

protected:
    KartAccessor *m_accessor;

private:
    u8 _0[0xc - 0x4];
};
static_assert(sizeof(KartObjectProxy) == 0xc);

} // namespace Kart
