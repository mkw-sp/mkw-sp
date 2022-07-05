#include "RaceManager.hh"

namespace System {

PadProxy *RaceManager::Player::padProxy() {
    return m_padProxy;
}

RaceManager::Player *RaceManager::player(u32 playerId) {
    return m_players[playerId];
}

RaceManager *RaceManager::Instance() {
    return s_instance;
}

} // namespace System
