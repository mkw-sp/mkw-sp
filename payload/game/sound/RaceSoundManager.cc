#include "RaceSoundManager.hh"

#include <game/system/RaceConfig.hh>

namespace Sound {

u32 RaceSoundManager::state() const {
    return m_state;
}

u32 RaceSoundManager::getBGMSoundId() {
    auto &packInfo = System::RaceConfig::Instance()->m_packInfo;
    auto trackMusic = packInfo.getSelectedMusic();

    if (trackMusic.has_value()) {
        return *trackMusic;
    } else {
        return REPLACED(getBGMSoundId)();
    }
}

RaceSoundManager *RaceSoundManager::Instance() {
    return s_instance;
}

} // namespace Sound
