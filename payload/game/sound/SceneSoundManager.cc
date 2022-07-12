#include "SceneSoundManager.hh"

namespace Sound {

SceneSoundManager *SceneSoundManager::Instance() {
    return s_instance;
}

} // namespace Sound
