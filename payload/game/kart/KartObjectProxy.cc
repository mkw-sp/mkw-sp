#include "KartObjectProxy.hh"

#include "game/kart/VehiclePhysics.hh"

namespace Kart {

const Quat<f32> *KartObjectProxy::getMainRot() const {
    return &getVehiclePhysics()->m_mainRot;
}

KartState *KartObjectProxy::getKartState() {
    return m_accessor->state;
}

const KartRollback *KartObjectProxy::getKartRollback() const {
    return m_accessor->rollback;
}

} // namespace Kart
