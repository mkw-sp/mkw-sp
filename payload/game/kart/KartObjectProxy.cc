#include "KartObjectProxy.hh"

#include "game/kart/KartCollide.hh"
#include "game/kart/KartMove.hh"
#include "game/kart/VehiclePhysics.hh"
#include "game/sound/KartSound.hh"

namespace Kart {

const Quat *KartObjectProxy::getMainRot() const {
    return &getVehiclePhysics()->m_mainRot;
}

KartState *KartObjectProxy::getKartState() {
    return m_accessor->state;
}

PlayerModel *KartObjectProxy::getPlayerModel() {
    return m_accessor->playerModel;
}

Sound::KartSound *KartObjectProxy::getKartSound() {
    return m_accessor->sound;
}

const KartRollback *KartObjectProxy::getKartRollback() const {
    return m_accessor->rollback;
}

s16 KartObjectProxy::getTimeBeforeRespawn() const {
    return m_accessor->collide->m_timeBeforeRespawn;
}

u16 KartObjectProxy::getTimeInRespawn() const {
    return m_accessor->move->m_timeInRespawn;
}

s16 KartObjectProxy::getTimeBeforeBoostEnd(u8 type) const {
    return m_accessor->move->m_boost.m_timesBeforeEnd[type];
}

} // namespace Kart
