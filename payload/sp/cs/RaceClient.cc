#include "RaceClient.hh"

#include "sp/cs/RoomClient.hh"

#include <game/system/RaceConfig.hh>
#include <game/system/RaceManager.hh>
#include <vendor/nanopb/pb_decode.h>
#include <vendor/nanopb/pb_encode.h>

namespace SP {

void RaceClient::destroyInstance() {
    DestroyInstance();
}

void RaceClient::calcWrite() {
    auto &raceScenario = System::RaceConfig::Instance()->raceScenario();
    RaceClientFrame frame;
    frame.id = m_frameId++;
    frame.players_count = raceScenario.localPlayerCount;
    for (u8 i = 0; i < raceScenario.localPlayerCount; i++) {
        u8 playerId = raceScenario.screenPlayerIds[i];
        auto *player = System::RaceManager::Instance()->player(playerId);
        auto &input = player->padProxy()->currentRaceInputState();
        frame.players[i].accelerate = input.accelerate;
        frame.players[i].brake = input.brake;
        frame.players[i].item = input.item;
        frame.players[i].drift = input.drift;
        frame.players[i].brakeDrift = input.brakeDrift;
        frame.players[i].stickX = input.rawStick.x;
        frame.players[i].stickY = input.rawStick.y;
        frame.players[i].trick = input.rawTrick;
    }

    u8 buffer[RaceClientFrame_size];
    pb_ostream_t stream = pb_ostream_from_buffer(buffer, sizeof(buffer));

    assert(pb_encode(&stream, RaceClientFrame_fields, &frame));

    m_socket.write(buffer, stream.bytes_written, m_connection);
}

void RaceClient::calcRead() {
    ConnectionGroup connectionGroup(*this);

    u8 buffer[RaceServerFrame_size];
    auto read = m_socket.read(buffer, sizeof(buffer), connectionGroup);
    // TODO only return when we get EAGAIN
    if (!read || read->size == 0) {
        return;
    }

    pb_istream_t stream = pb_istream_from_buffer(buffer, read->size);

    RaceServerFrame frame;
    if (!pb_decode(&stream, RaceServerFrame_fields, &frame)) {
        return;
    }

    System::RaceManager::Instance()->m_canStartCountdown = true;

    if (!m_frame || frame.id > m_frame->id ||
            (frame.id == m_frame->id && frame.clientId > m_frame->clientId)) {
        m_frame = frame;
    }

    if (m_drifts.full()) {
        m_drifts.pop();
    }
    s32 drift = 0;
    drift += System::RaceManager::Instance()->frameId();
    drift -= m_frameId;
    drift -= m_frame->id;
    drift += m_frame->clientId;
    m_drifts.push(std::move(drift));

    m_drift = 0;
    for (size_t i = 0; i < m_drifts.count(); i++) {
        m_drift += *m_drifts[i];
    }
    m_drift /= static_cast<s32>(m_drifts.count());
}

RaceClient *RaceClient::CreateInstance() {
    assert(s_block);
    assert(!s_instance);
    auto *roomClient = RoomClient::Instance();
    u32 ip = roomClient->ip();
    u16 port = roomClient->port();
    hydro_kx_session_keypair keypair = roomClient->keypair();
    s_instance = new (s_block) RaceClient(ip, port, keypair);
    RaceManager::s_instance = s_instance;
    return s_instance;
}

void RaceClient::DestroyInstance() {
    assert(s_instance);
    s_instance->~RaceClient();
    RaceManager::s_instance = nullptr;
    s_instance = nullptr;
}

RaceClient *RaceClient::Instance() {
    return s_instance;
}

RaceClient::RaceClient(u32 ip, u16 port, hydro_kx_session_keypair keypair) :
        m_connection{ip, port, keypair}, m_socket("race    ", {}) {}

RaceClient::~RaceClient() {
    hydro_memzero(&m_connection, sizeof(m_connection));
}

RaceClient::ConnectionGroup::ConnectionGroup(RaceClient &client) : m_client(client) {}

u32 RaceClient::ConnectionGroup::count() {
    return 1;
}

Net::UnreliableSocket::Connection &RaceClient::ConnectionGroup::operator[](u32 index) {
    assert(index == 0);
    return m_client.m_connection;
}


RaceClient *RaceClient::s_instance = nullptr;

} // namespace SP
