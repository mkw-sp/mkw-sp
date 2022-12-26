#include "EnemyManager.hh"

#include <sp/cs/RoomManager.hh>

namespace Enemy {

EnemyManager *EnemyManager::Instance() {
    return s_instance;
}

bool EnemyManager::hasCpus() const {
    if (SP::RoomManager::Instance()) {
        return false;
    }

    return REPLACED(hasCpus)();
}

} // namespace Enemy
