#include "RaceServer.hh"

namespace SP {

void RaceServer::destroyInstance() {
    DestroyInstance();
}

RaceServer *RaceServer::CreateInstance(
        std::array<Net::UnreliableSocket::ConnectionInfo, 24> &connections, u32 connectionCount,
        u16 port) {
    assert(s_block);
    assert(!s_instance);
    s_instance = new (s_block) RaceServer(connections, connectionCount, port);
    RaceManager::s_instance = s_instance;
    return s_instance;
}

void RaceServer::DestroyInstance() {
    assert(s_instance);
    s_instance->~RaceServer();
    RaceManager::s_instance = nullptr;
    s_instance = nullptr;
}

RaceServer *RaceServer::Instance() {
    return s_instance;
}

RaceServer::RaceServer(std::array<Net::UnreliableSocket::ConnectionInfo, 24> &connections,
        u32 connectionCount, u16 port)
    : m_socket(connections, "race    ", connectionCount, port) {}

RaceServer::~RaceServer() = default;

RaceServer *RaceServer::s_instance = nullptr;

} // namespace SP
