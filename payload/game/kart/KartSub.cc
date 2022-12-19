#include "KartSub.hh"

#include <sp/cs/RoomManager.hh>

namespace Kart {

void KartSub::createComponents(KartSettings *settings) {
    REPLACED(createComponents)(settings);

    if (auto *roomManager = SP::RoomManager::Instance(); roomManager &&
            !roomManager->isPlayerLocal(settings->playerId)) {
        m_state->m_isCpu = false;
    }
}

} // namespace Kart
