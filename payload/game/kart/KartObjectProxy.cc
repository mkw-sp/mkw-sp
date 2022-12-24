#include "KartObjectProxy.hh"

#include "game/kart/VehiclePhysics.hh"

namespace Kart {

const Quat *KartObjectProxy::getMainRot() const {
    return &getVehiclePhysics()->m_mainRot;
}

KartState *KartObjectProxy::getKartState() {
    return m_accessor->state;
}

} // namespace Kart
