#include "RaceClient.hh"

#include "sp/cs/RoomClient.hh"

#include <game/kart/KartObjectManager.hh>
#include <game/system/RaceConfig.hh>
#include <game/system/RaceManager.hh>
#include <vendor/nanopb/pb_decode.h>
#include <vendor/nanopb/pb_encode.h>

#include <cmath>

namespace SP {

void RaceClient::destroyInstance() {
    DestroyInstance();
}

u32 RaceClient::frameCount() const {
    return m_frameCount;
}

const std::optional<RaceServerFrame> &RaceClient::frame() const {
    return m_frame;
}

/*s32 RaceClient::drift() const {
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
}*/

void RaceClient::calcWrite() {
    if (!m_frame) {
        u8 buffer[RaceClientPing_size];
        pb_ostream_t stream = pb_ostream_from_buffer(buffer, sizeof(buffer));

        assert(pb_encode(&stream, RaceClientPing_fields, nullptr));

        m_socket.write(buffer, stream.bytes_written, m_connection);
    }

    auto &raceScenario = System::RaceConfig::Instance()->raceScenario();
    RoomRequest request;
    request.which_request = RoomRequest_race_tag;
    request.request.race.time = System::RaceManager::Instance()->time();
    request.request.race.serverTime = m_frame ? m_frame->time : 0;
    request.request.race.players_count = raceScenario.localPlayerCount;
    for (u8 i = 0; i < raceScenario.localPlayerCount; i++) {
        u8 playerId = raceScenario.screenPlayerIds[i];
        auto *player = System::RaceManager::Instance()->player(playerId);
        auto &inputState = player->padProxy()->currentRaceInputState();
        request.request.race.players[i].inputState.accelerate = inputState.accelerate;
        request.request.race.players[i].inputState.brake = inputState.brake;
        request.request.race.players[i].inputState.item = inputState.item;
        request.request.race.players[i].inputState.drift = inputState.drift;
        request.request.race.players[i].inputState.brakeDrift = inputState.brakeDrift;
        request.request.race.players[i].inputState.stickX = inputState.rawStick.x;
        request.request.race.players[i].inputState.stickY = inputState.rawStick.y;
        request.request.race.players[i].inputState.trick = inputState.rawTrick;
        auto *object = Kart::KartObjectManager::Instance()->object(playerId);
        request.request.race.players[i].timeBeforeRespawn = object->getTimeBeforeRespawn();
        request.request.race.players[i].timeInRespawn = object->getTimeInRespawn();
        request.request.race.players[i].timesBeforeBoostEnd_count = 3;
        for (u32 j = 0; j < 3; j++) {
            request.request.race.players[i].timesBeforeBoostEnd[j] =
                    object->getTimeBeforeBoostEnd(j * 2);
        }
        request.request.race.players[i].pos = *object->getPos();
        request.request.race.players[i].mainRot = *object->getMainRot();
        request.request.race.players[i].internalSpeed = object->getInternalSpeed();
    }

    u8 buffer[RoomRequest_size];
    pb_ostream_t stream = pb_ostream_from_buffer(buffer, sizeof(buffer));

    assert(pb_encode(&stream, RoomRequest_fields, &request));

    // TODO proper error handling
    m_roomClient.socket().write(buffer, stream.bytes_written);
    if (!m_roomClient.socket().poll()) {
        m_roomClient.handleError(30001);
    };
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
            m_frameCount++;
            m_frame = frame;
        }
    }

    if (!m_frame) {
        return;
    }

    System::RaceManager::Instance()->m_canStartCountdown = true;

    /*if (m_drifts.full()) {
        m_drifts.pop_front();
    }
    s32 drift = static_cast<s32>(m_frame->clientTime) - static_cast<s32>(m_frame->time);
    m_drifts.push_back(std::move(drift));

    m_drift = 0;
    for (size_t i = 0; i < m_drifts.count(); i++) {
        m_drift += *m_drifts[i];
    }
    m_drift /= static_cast<s32>(m_drifts.count());*/
}

RaceClient *RaceClient::CreateInstance() {
    assert(s_block);
    assert(!s_instance);
    auto *roomClient = RoomClient::Instance();
    s_instance = new (s_block) RaceClient(*roomClient);
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

RaceClient::RaceClient(RoomClient &roomClient)
    : m_roomClient(roomClient),
      m_socket("race    ", {}), m_connection{roomClient.ip(), roomClient.port(),
                                        roomClient.keypair()} {}

RaceClient::~RaceClient() {
    hydro_memzero(&m_connection, sizeof(m_connection));
}

bool RaceClient::isFrameValid(const RaceServerFrame &frame) {
    if (m_frame && frame.time <= m_frame->time) {
        return false;
    }

    // TODO check player times
    if (frame.playerTimes_count != m_roomClient.playerCount()) {
        return false;
    }
    if (m_frame) {
        for (u32 i = 0; i < frame.players_count; i++) {
            if (frame.playerTimes[i] < m_frame->playerTimes[i]) {
                return false;
            }
        }
    }

    if (frame.players_count != m_roomClient.playerCount()) {
        return false;
    }
    for (u32 i = 0; i < frame.players_count; i++) {
        if (!IsInputStateValid(frame.players[i].inputState)) {
            return false;
        }

        if (frame.players[i].timeBeforeRespawn > 190) {
            return false;
        }

        if (frame.players[i].timeInRespawn > 140) {
            return false;
        }

        if (frame.players[i].timeBeforeRespawn && frame.players[i].timeInRespawn) {
            return false;
        }

        if (frame.players[i].timesBeforeBoostEnd_count != 3) {
            return false;
        }
        for (u32 j = 0; j < 3; j++) {
            if (frame.players[i].timesBeforeBoostEnd[j] > 180) {
                return false;
            }
        }

        if (!IsVec3Valid(frame.players[i].pos)) {
            return false;
        }

        if (!IsQuatValid(frame.players[i].mainRot)) {
            return false;
        }

        if (!IsF32Valid(frame.players[i].internalSpeed)) {
            return false;
        }
    }

    return true;
}

bool RaceClient::IsVec3Valid(const PlayerFrame_Vec3 &v) {
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

bool RaceClient::IsQuatValid(const PlayerFrame_Quat &q) {
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

bool RaceClient::IsF32Valid(f32 s) {
    return !std::isnan(s) && s >= -20.0f && s <= 120.0f;
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
