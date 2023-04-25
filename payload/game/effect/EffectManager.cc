#include "EffectManager.hh"

#include "game/system/RaceConfig.hh"

#include <sp/cs/RoomManager.hh>

namespace Effect {

void EffectManager::createKarts() {
    REPLACED(createKarts)();

    if (auto *roomManager = SP::RoomManager::Instance()) {
        const auto &raceScenario = System::RaceConfig::Instance()->raceScenario();
        for (u32 i = 0; i < raceScenario.playerCount; i++) {
            if (roomManager->isPlayerRemote(i)) {
                m_karts[i]->m_isLod = true;
            }
        }
    }
}

KartEffect *EffectManager::getKartEffect(u32 i) const {
    return m_karts[i];
}

EffectManager *EffectManager::Instance() {
    return s_instance;
}

} // namespace Effect
