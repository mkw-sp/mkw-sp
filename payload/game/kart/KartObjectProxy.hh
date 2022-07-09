#pragma once

#include "game/kart/KartMove.hh"
#include "game/kart/KartState.hh"
#include "game/kart/VehiclePhysics.hh"

namespace Kart {

struct KartAccessor {
    u8 _00[0x04 - 0x00];
    KartState *state;
    u8 _08[0x64 - 0x08];
};
static_assert(sizeof(KartAccessor) == 0x64);

class KartObjectProxy {
public:
    const Vec3 *getPos() const;
    const Vec3 *getLastPos() const;
    KartState *getKartState();
    VehiclePhysics *getVehiclePhysics();
    bool isGhost() const;
    KartMove *getKartMove();
    u32 getPlayerId() const;
    f32 getInternalSpeed() const;

private:
    KartAccessor *m_accessor;
    u8 _0[0xc - 0x4];
};
static_assert(sizeof(KartObjectProxy) == 0xc);

} // namespace Kart
