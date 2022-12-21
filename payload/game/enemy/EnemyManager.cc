#include "EnemyManager.hh"

namespace Enemy {

EnemyManager *EnemyManager::Instance() {
    return s_instance;
}

} // namespace Enemy
