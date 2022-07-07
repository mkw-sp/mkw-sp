#include "VehiclePhysics.hh"

namespace Kart {

const Vec3 *VehiclePhysics::movingRoadVel() const {
    return &m_movingRoadVel;
}

const Vec3 *VehiclePhysics::movingWaterVel() const {
    return &m_movingWaterVel;
}

} // namespace Kart
