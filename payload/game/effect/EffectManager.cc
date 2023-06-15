#include "EffectManager.hh"

#include "game/system/RaceConfig.hh"
#include "game/system/ResourceManager.hh"

#include <sp/cs/RoomManager.hh>

namespace Effect {

EGG::EffectResource *EffectManager::s_raceSPResource{};

void EffectManager::deinit() {
    delete s_raceSPResource;
    s_raceSPResource = nullptr;

    REPLACED(deinit)();
}

void EffectManager::initRace() {
    REPLACED(initRace)();

    auto *resourceManager = System::ResourceManager::Instance();
    auto *breff = resourceManager->getFile(System::ResChannelId::Race, "RKRace_SP.breff", nullptr);
    auto *breft = resourceManager->getFile(System::ResChannelId::Race, "RKRace_SP.breft", nullptr);
    s_raceSPResource = new EGG::EffectResource(breff, breft);
}

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
