#include "KartMove.hh"

#include "game/kart/KartState.hh"

#include <sp/ThumbnailManager.hh>

namespace Kart {

f32 KartMove::hardSpeedLimit() const {
    return m_hardSpeedLimit;
}

const Vec3 *KartMove::internalVelDir() const {
    return &m_internalVelDir;
}

void KartMove::calcBlink() {
    if (SP::ThumbnailManager::IsActive()) {
        m_blinkTimer = 150;
        getKartState()->m_blinking = true;
    }

    REPLACED(calcBlink)();
}

} // namespace Kart
