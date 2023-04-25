#pragma once

#include <common/TQuat.hh>
#include <common/TVec3.hh>

namespace Sound {
class KartSound;
}

namespace Kart {

class Kart5c;
class KartAction;
class KartBody;
class KartCollide;
class KartMove;
class KartRollback;
class KartSettings;
class KartState;
class KartSub;
class KartSus;
class KartTire;
class PlayerModel;
class VehiclePhysics;

struct KartAccessor {
    KartSettings *settings;
    KartState *state;
    KartBody *body;
    KartSus **sus;
    KartTire **tire;
    PlayerModel *playerModel;
    KartSub *sub;
    Sound::KartSound *sound;
    u8 _1c[0x28 - 0x20];
    KartMove *move;
    KartAction *action;
    KartCollide *collide;
    u8 _34[0x3c - 0x34];
    KartRollback *rollback; // Replaced
    u8 _40[0x5c - 0x40];
    Kart5c *unk5c;
    u8 _44[0x64 - 0x60];
};

static_assert(sizeof(KartAccessor) == 0x64);

class KartObjectProxy {
public:
    KartObjectProxy();

    const Vec3 *getPos() const;
    const Vec3 *getLastPos() const;
    const Quat *getMainRot() const;
    KartState *getKartState();
    PlayerModel *getPlayerModel();
    Sound::KartSound *getKartSound();
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
