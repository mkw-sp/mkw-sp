#include "RaceSoundManager.hh"

#include <game/system/RaceConfig.hh>

namespace Sound {

u32 RaceSoundManager::state() const {
    return m_state;
}

u32 RaceSoundManager::getBGMSoundId() {
    auto &packInfo = System::RaceConfig::Instance()->m_packInfo;
    return packInfo.getSelectedMusic();
}

RaceSoundManager *RaceSoundManager::Instance() {
    return s_instance;
}

} // namespace Sound
