#include "SceneSoundManager.hh"

namespace Sound {

nw4r::snd::SoundHandle &SceneSoundManager::Handle() {
    return s_handle;
}

SceneSoundManager *SceneSoundManager::Instance() {
    return s_instance;
}

} // namespace Sound
