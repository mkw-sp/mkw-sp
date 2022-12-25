#include "KartEnemy.hh"

#include <sp/cs/RoomManager.hh>

namespace Enemy {

bool KartEnemy::isCpu() {
    if (auto *roomManager = SP::RoomManager::Instance(); roomManager &&
            roomManager->isPlayerRemote(getPlayerId())) {
        return false;
    }

    return REPLACED(isCpu)();
}

} // namespace Enemy
