#include "CoinManager.hh"

namespace Battle {

CoinManager *CoinManager::Instance() {
    return s_instance;
}

} // namespace Battle
