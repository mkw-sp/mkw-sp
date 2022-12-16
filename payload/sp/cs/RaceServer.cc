#include "RaceServer.hh"

#include "sp/cs/RoomServer.hh"

namespace SP {

void RaceServer::destroyInstance() {
    DestroyInstance();
}

RaceServer *RaceServer::CreateInstance() {
    assert(s_block);
    assert(!s_instance);
    auto *roomServer = RoomServer::Instance();
    std::array<Net::UnreliableSocket::ConnectionInfo, 24> connections;
    u32 connectionCount = 0;
    for (u32 i = 0; i < 12; i++) {
        if (auto keypair = roomServer->clientKeypair(i)) {
            connections[connectionCount++] =
                    SP::Net::UnreliableSocket::ConnectionInfo{0, 0, *keypair};
        }
    }
    s_instance = new (s_block) RaceServer(connections, connectionCount, 21330);
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
