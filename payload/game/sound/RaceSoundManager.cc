#include "RaceSoundManager.hh"

#include "game/system/RaceConfig.hh"

namespace Sound {

u32 RaceSoundManager::state() const {
    return m_state;
}

RaceSoundManager *RaceSoundManager::Instance() {
    return s_instance;
}

SoundId RaceSoundManager::getBGMSoundId() const {
    auto &raceScenario = System::RaceConfig::Instance()->m_spRace;
    if (raceScenario.musicReplacement.has_value()) {
        return *raceScenario.musicReplacement;
    } else {
        return REPLACED(getBGMSoundId)();
    }
}

} // namespace Sound
