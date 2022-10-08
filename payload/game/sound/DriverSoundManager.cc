#include "DriverSoundManager.hh"

namespace Sound {

DriverSoundManager *DriverSoundManager::Instance() {
    return s_instance;
}

} // namespace Sound
