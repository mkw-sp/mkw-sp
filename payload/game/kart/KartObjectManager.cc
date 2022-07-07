#include "KartObjectManager.hh"

namespace Kart {

KartObjectManager *KartObjectManager::Instance() {
    return s_instance;
}

} // namespace Kart
