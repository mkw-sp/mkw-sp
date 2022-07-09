#include "RaceSoundManager.hh"

namespace Sound {

u32 RaceSoundManager::state() const {
    return m_state;
}

RaceSoundManager *RaceSoundManager::Instance() {
    return s_instance;
}

} // namespace Sound
