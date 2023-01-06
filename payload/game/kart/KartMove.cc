#include "KartMove.hh"

#include "game/kart/KartState.hh"
#include "game/system/SaveManager.hh"

#include <sp/ThumbnailManager.hh>
#include <sp/settings/ClientSettings.hh>

namespace Kart {

f32 KartMove::hardSpeedLimit() const {
    return m_hardSpeedLimit;
}

const Vec3 *KartMove::internalVelDir() const {
    return &m_internalVelDir;
}

void KartMove::calcBlink() {
    if (SP::ThumbnailManager::IsActive()) {
        m_timeBeforeBlinkEnd = 150;
        getKartState()->m_blinking = true;
    }

    REPLACED(calcBlink)();
}

bool KartMove::activateTcLightning() {
    auto *saveManager = System::SaveManager::Instance();
    auto setting = saveManager->getSetting<SP::ClientSettings::Setting::VSMegaClouds>();
    if (setting == SP::ClientSettings::VSMegaClouds::Enable) {
        KartMove::activateMega();
        return false;
    }

    return REPLACED(activateTcLightning)();
}

} // namespace Kart
