#include "RaceClient.hh"

#include "sp/cs/RoomClient.hh"

#include <game/system/RaceConfig.hh>
#include <game/system/RaceManager.hh>
#include <vendor/nanopb/pb_decode.h>
#include <vendor/nanopb/pb_encode.h>

#include <cmath>

namespace SP {

void RaceClient::destroyInstance() {
    DestroyInstance();
}

const std::optional<RaceServerFrame> &RaceClient::frame() const {
    return m_frame;
}

s32 RaceClient::drift() const {
    return m_drift;
}

void RaceClient::adjustDrift() {
    if (m_drift == 0) {
        return;
    }

    s32 signum = (m_drift > 0) - (m_drift < 0);
    for (size_t i = 0; i < m_drifts.count(); i++) {
        *m_drifts[i] -= signum;
    }
    m_drift -= signum;
}

void RaceClient::calcWrite() {
    auto &raceScenario = System::RaceConfig::Instance()->raceScenario();
    RaceClientFrame frame;
    frame.id = System::RaceManager::Instance()->frameId();
    frame.players_count = raceScenario.localPlayerCount;
    for (u8 i = 0; i < raceScenario.localPlayerCount; i++) {
        u8 playerId = raceScenario.screenPlayerIds[i];
        auto *player = System::RaceManager::Instance()->player(playerId);
        auto &inputState = player->padProxy()->currentRaceInputState();
        frame.players[i].inputState.accelerate = inputState.accelerate;
        frame.players[i].inputState.brake = inputState.brake;
        frame.players[i].inputState.item = inputState.item;
        frame.players[i].inputState.drift = inputState.drift;
        frame.players[i].inputState.brakeDrift = inputState.brakeDrift;
        frame.players[i].inputState.stickX = inputState.rawStick.x;
        frame.players[i].inputState.stickY = inputState.rawStick.y;
        frame.players[i].inputState.trick = inputState.rawTrick;
    }

    u8 buffer[RaceClientFrame_size];
    pb_ostream_t stream = pb_ostream_from_buffer(buffer, sizeof(buffer));

    assert(pb_encode(&stream, RaceClientFrame_fields, &frame));

    m_socket.write(buffer, stream.bytes_written, m_connection);
}

void RaceClient::calcRead() {
    ConnectionGroup connectionGroup(*this);

    while (true) {
        u8 buffer[RaceServerFrame_size];
        auto read = m_socket.read(buffer, sizeof(buffer), connectionGroup);
        if (!read) {
            break;
        }

        pb_istream_t stream = pb_istream_from_buffer(buffer, read->size);

        RaceServerFrame frame;
        if (!pb_decode(&stream, RaceServerFrame_fields, &frame)) {
            continue;
        }

        if (isFrameValid(frame)) {
            m_frame = frame;
        }
    }

    if (!m_frame) {
        return;
    }

    System::RaceManager::Instance()->m_canStartCountdown = true;

    if (m_drifts.full()) {
        m_drifts.pop();
    }
    s32 drift = static_cast<s32>(m_frame->clientId) - static_cast<s32>(m_frame->id);
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

bool RaceClient::isFrameValid(const RaceServerFrame &frame) {
    if (m_frame && frame.id <= m_frame->id) {
        return false;
    }

    for (u32 i = 0; i < frame.players_count; i++) {
        if (!IsInputStateValid(frame.players[i].inputState)) {
            return false;
        }

        if (!IsVec3Valid(frame.players[i].pos)) {
            return false;
        }

        if (!IsQuatValid(frame.players[i].mainRot)) {
            return false;
        }
    }

    return true;
}

bool RaceClient::IsVec3Valid(const RaceServerFrame_Vec3 &v) {
    if (std::isnan(v.x) || v.x < -1e6f || v.x > 1e6f) {
        return false;
    }

    if (std::isnan(v.y) || v.y < -1e6f || v.y > 1e6f) {
        return false;
    }

    if (std::isnan(v.z) || v.z < -1e6f || v.z > 1e6f) {
        return false;
    }

    return true;
}

bool RaceClient::IsQuatValid(const RaceServerFrame_Quat &q) {
    if (std::isnan(q.x) || q.x < -1.001f || q.x > 1.001f) {
        return false;
    }

    if (std::isnan(q.y) || q.y < -1.001f || q.y > 1.001f) {
        return false;
    }

    if (std::isnan(q.z) || q.z < -1.001f || q.z > 1.001f) {
        return false;
    }

    if (std::isnan(q.w) || q.w < -1.001f || q.w > 1.001f) {
        return false;
    }

    return true;
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
