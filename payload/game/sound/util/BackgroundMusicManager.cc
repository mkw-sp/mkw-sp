#include "BackgroundMusicManager.hh"

namespace Sound {

BackgroundMusicManager *BackgroundMusicManager::Instance() {
    return s_instance;
}

} // namespace Sound
