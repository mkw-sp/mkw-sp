#include "KartMove.hh"

namespace Kart {

f32 KartMove::hardSpeedLimit() const {
    return m_hardSpeedLimit;
}

const Vec3 *KartMove::internalVelDir() const {
    return &m_internalVelDir;
}

} // namespace Kart
