#include "KartMove.hh"

#include "game/kart/KartState.hh"

extern "C" {
#include "game/system/SaveManager.h"
}
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
        m_blinkTimer = 150;
        getKartState()->m_blinking = true;
    }

    REPLACED(calcBlink)();
}

bool KartMove::ThunderActive(int timer, int param_3, int param_4) {
    auto *saveManager = System::SaveManager::Instance();
    auto setting = saveManager->getSetting<SP::ClientSettings::Setting::VSMegaClouds>();
    if(setting == SP::ClientSettings::VSMegaClouds::Enable) {
        KartMove::ActivateMega();
        return false;
    }

    REPLACED(ThunderActive)(timer, param_3, param_4);

    return true;
}

} // namespace Kart
