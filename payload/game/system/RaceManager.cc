#include "RaceManager.hh"

namespace System {

u8 RaceManager::Player::maxLap() const {
    return m_maxLap;
}

bool RaceManager::Player::hasFinished() const {
    return m_hasFinished;
}

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
