#include "RaceServer.hh"

#include "sp/cs/RoomServer.hh"

#include <game/kart/KartObjectManager.hh>
#include <game/system/RaceManager.hh>
#include <vendor/nanopb/pb_decode.h>
#include <vendor/nanopb/pb_encode.h>

namespace SP {

void RaceServer::destroyInstance() {
    DestroyInstance();
}

void RaceServer::calcRead() {
    ConnectionGroup connectionGroup(*this);

    while (true) {
        u8 buffer[RaceClientFrame_size];
        auto read = m_socket.read(buffer, sizeof(buffer), connectionGroup);
        if (!read) {
            break;
        }

        pb_istream_t stream = pb_istream_from_buffer(buffer, read->size);

        RaceClientFrame frame;
        if (!pb_decode(&stream, RaceClientFrame_fields, &frame)) {
            continue;
        }

        u32 clientId = connectionGroup.clientId(read->index);
        assert(m_clients[clientId]);
        if (isFrameValid(frame, clientId)) {
            m_clients[clientId]->frame = frame;
        }
    }

    for (u32 i = 0; i < m_playerCount; i++) {
        if (!m_clients[m_players[i].clientId]->frame) {
            return;
        }
    }

    System::RaceManager::Instance()->m_canStartCountdown = true;

    if (System::RaceManager::Instance()->hasReachedStage(System::RaceManager::Stage::Countdown)) {
        for (u32 i = 0; i < m_playerCount; i++) {
            // TODO 2P
            auto framePlayer = m_clients[m_players[i].clientId]->frame->players[0];
            auto *pad = System::InputManager::Instance()->extraUserPad(i);
            pad->m_userInputState.accelerate = framePlayer.accelerate;
            pad->m_userInputState.brake = framePlayer.brake;
            pad->m_userInputState.item = framePlayer.item;
            pad->m_userInputState.drift = framePlayer.drift;
            pad->m_userInputState.brakeDrift = framePlayer.brakeDrift; // TODO check for 200cc
            System::RaceInputState::SetStickX(pad->m_userInputState, framePlayer.stickX);
            System::RaceInputState::SetStickY(pad->m_userInputState, framePlayer.stickY);
            System::RaceInputState::SetTrick(pad->m_userInputState, framePlayer.trick);
        }
    }
}

void RaceServer::calcWrite() {
    if (!System::RaceManager::Instance()->m_canStartCountdown) {
        return;
    }

    RaceServerFrame frame;
    frame.players_count = m_playerCount;
    for (u32 i = 0; i < m_playerCount; i++) {
        auto *object = Kart::KartObjectManager::Instance()->object(i);
        auto *pos = object->getPos();
        frame.players[i].pos.x = pos->x;
        frame.players[i].pos.y = pos->y;
        frame.players[i].pos.z = pos->z;
        auto *mainRot = object->getMainRot();
        frame.players[i].mainRot.x = mainRot->x;
        frame.players[i].mainRot.y = mainRot->y;
        frame.players[i].mainRot.z = mainRot->z;
        frame.players[i].mainRot.w = mainRot->w;
    }
    for (u32 i = 0; i < 12; i++) {
        if (m_clients[i] && m_clients[i]->frame) {
            frame.id = System::RaceManager::Instance()->frameId();
            frame.clientId = m_clients[i]->frame->id;

            u8 buffer[RaceServerFrame_size];
            pb_ostream_t stream = pb_ostream_from_buffer(buffer, sizeof(buffer));

            assert(pb_encode(&stream, RaceServerFrame_fields, &frame));

            m_socket.write(buffer, stream.bytes_written, m_clients[i]->connection);
        }
    }
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

bool RaceServer::isFrameValid(const RaceClientFrame &frame, u32 clientId) {
    if (m_clients[clientId]->frame && frame.id <= m_clients[clientId]->frame->id) {
        return false;
    }

    for (u32 i = 0; i < frame.players_count; i++) {
        if (frame.players[i].stickX > 14) {
            return false;
        }

        if (frame.players[i].stickY > 14) {
            return false;
        }

        if (frame.players[i].trick > 4) {
            return false;
        }
    }

    return true;
}

RaceServer::ConnectionGroup::ConnectionGroup(RaceServer &server) : m_server(server) {
    for (u32 i = 0; i < 12; i++) {
        if (m_server.m_clients[i]) {
            m_clientIds[m_clientCount++] = i;
        }
    }
}

u32 RaceServer::ConnectionGroup::clientId(u32 index) const {
    return m_clientIds[index];
}

u32 RaceServer::ConnectionGroup::count() {
    return m_clientCount;
}

Net::UnreliableSocket::Connection &RaceServer::ConnectionGroup::operator[](u32 index) {
    return m_server.m_clients[m_clientIds[index]]->connection;
}

RaceServer *RaceServer::s_instance = nullptr;

} // namespace SP
