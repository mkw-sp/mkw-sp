#include "RaceServer.hh"

#include "sp/cs/RoomServer.hh"

#include <game/system/RaceManager.hh>
#include <vendor/nanopb/pb_decode.h>
#include <vendor/nanopb/pb_encode.h>

namespace SP {

void RaceServer::destroyInstance() {
    DestroyInstance();
}

void RaceServer::calcRead() {
    std::array<u32, 12> clientIds{};
    u32 connectionCount = 0;
    std::array<Net::UnreliableSocket::Connection, 12> connections{};
    for (u32 i = 0; i < 12; i++) {
        if (m_clients[i]) {
            clientIds[connectionCount] = i;
            connections[connectionCount] = m_clients[i]->connection;
            connectionCount++;
        }
    }

    while (true) {
        u8 buffer[RaceClientFrame_size];
        auto read = m_socket.read(buffer, sizeof(buffer), {connections.begin(), connectionCount});
        // TODO only break when we get EAGAIN
        if (!read || read->size == 0) {
            break;
        }

        pb_istream_t stream = pb_istream_from_buffer(buffer, read->size);

        RaceClientFrame frame;
        if (!pb_decode(&stream, RaceClientFrame_fields, &frame)) {
            continue;
        }

        u32 clientId = clientIds[read->playerIdx];
        assert(m_clients[clientId]);
        if (!m_clients[clientId]->frame || frame.id > m_clients[clientId]->frame->id) {
            m_clients[clientId]->frame = frame;
        }
    }

    for (u32 i = 0; i < m_playerCount; i++) {
        if (!m_clients[m_players[i].clientId]->frame) {
            return;
        }
    }

    System::RaceManager::Instance()->m_canStartCountdown = true;
}

RaceServer *RaceServer::CreateInstance() {
    assert(s_block);
    assert(!s_instance);
    auto *roomServer = RoomServer::Instance();
    std::array<std::optional<Client>, 12> clients{};
    for (u32 i = 0; i < 12; i++) {
        if (auto keypair = roomServer->clientKeypair(i)) {
            clients[i] = {{}, {0, 0, *keypair}};
        }
    }
    s_instance = new (s_block) RaceServer(clients, 21330);
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

RaceServer::RaceServer(std::array<std::optional<Client>, 12> clients, u16 port)
    : m_clients(clients), m_socket("race    ", port) {}

RaceServer::~RaceServer() {
    hydro_memzero(&m_clients, sizeof(m_clients));
}

RaceServer *RaceServer::s_instance = nullptr;

} // namespace SP
