#include "RaceManager.hh"

namespace SP {

RaceManager::RaceManager() : m_roomManager(*RoomManager::Instance()) {}

RaceManager::~RaceManager() = default;

void *RaceManager::s_block = nullptr;
RaceManager *RaceManager::s_instance = nullptr;

} // namespace SP
