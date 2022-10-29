#include "RoomClient.hh"

#include <egg/core/eggHeap.hh>
extern "C" {
#include <game/system/RootScene.h>
}
#include <game/system/SaveManager.hh>
#include <game/ui/SectionManager.hh>
#include <vendor/nanopb/pb_decode.h>
#include <vendor/nanopb/pb_encode.h>

namespace SP {

bool RoomClient::calc(Handler &handler) {
    if (auto state = resolve(handler)) {
        if (!transition(handler, *state)) {
            return false;
        }
    } else {
        return false;
    }

    if (!m_socket.poll()) {
        return false;
    }

    if (auto state = resolve(handler)) {
        if (!transition(handler, *state)) {
            return false;
        }
    } else {
        return false;
    }

    return true;
}

bool RoomClient::sendComment(u32 commentId) {
    return writeComment(commentId);
}

bool RoomClient::closeRoom(u32 gamemode) {
    return writeClose(gamemode);
}

void RoomClient::changeLocalSettings() {
    m_localSettingsChanged = true;
}

bool RoomClient::sendVote(u32 course, std::optional<Player::Properties> properties) {
    return writeVote(course, properties);
}

RoomClient *RoomClient::CreateInstance(u32 localPlayerCount) {
    assert(s_block);
    assert(!s_instance);
    s_instance = new (s_block) RoomClient(localPlayerCount);
    RoomManager::s_instance = s_instance;
    return s_instance;
}

void RoomClient::DestroyInstance() {
    assert(s_instance);
    s_instance->~RoomClient();
    RoomManager::s_instance = nullptr;
    s_instance = nullptr;
}

RoomClient *RoomClient::Instance() {
    return s_instance;
}

RoomClient::RoomClient(u32 localPlayerCount)
    : m_localPlayerCount(localPlayerCount), m_state(State::Connect),
      m_socket(0x7f000001, 21330, "room    ") {}

RoomClient::~RoomClient() = default;

std::optional<RoomClient::State> RoomClient::resolve(Handler &handler) {
    switch (m_state) {
    case State::Connect:
        return calcConnect();
    case State::Setup:
        return calcSetup(handler);
    case State::Main:
        return calcMain(handler);
    case State::Select:
        return calcSelect(handler);
    }

    return m_state;
}

bool RoomClient::transition(Handler &handler, State state) {
    if (state == m_state) {
        return true;
    }

    bool result = true;
    switch (state) {
    case State::Connect:
        break;
    case State::Setup:
        result = onSetup(handler);
        break;
    case State::Main:
        result = onMain(handler);
        break;
    case State::Select:
        break;
    }
    m_state = state;
    return result;
}

std::optional<RoomClient::State> RoomClient::calcConnect() {
    if (!m_socket.ready()) {
        return State::Connect;
    }

    return State::Setup;
}

std::optional<RoomClient::State> RoomClient::calcSetup(Handler &handler) {
    std::optional<RoomEvent> event{};
    if (!read(event)) {
        return {};
    }

    if (!event) {
        return State::Setup;
    }

    switch (event->which_event) {
    case RoomEvent_join_tag:
        if (event->event.join.mii.size != sizeof(System::RawMii)) {
            return {};
        } else {
            auto *mii = reinterpret_cast<System::RawMii *>(event->event.join.mii.bytes);
            u32 location = event->event.join.location;
            u16 latitude = event->event.join.latitude;
            u16 longitude = event->event.join.longitude;
            u32 regionLineColor = event->event.join.regionLineColor;
            if (!onPlayerJoin(handler, mii, location, latitude, longitude, regionLineColor)) {
                return {};
            }
        }
        return State::Setup;
    case RoomEvent_settings_tag:
        if (m_playerCount == 0) {
            auto *saveManager = System::SaveManager::Instance();
            for (size_t i = 0; i < RoomSettings::count; i++) {
                m_players[m_localPlayerIds[0]].m_settings[i] =
                        saveManager->getSetting(RoomSettings::offset + i);
            }
        } else {
            if (event->event.settings.settings_count != RoomSettings::count) {
                return {};
            }
            for (size_t i = 0; i < RoomSettings::count; i++) {
                m_players[m_localPlayerIds[0]].m_settings[i] = event->event.settings.settings[i];
            }
        }
        handler.onSettingsChange(m_players[m_localPlayerIds[0]].m_settings);
        return State::Main;
    default:
        return {};
    }
}

std::optional<RoomClient::State> RoomClient::calcMain(Handler &handler) {
    if (m_localSettingsChanged) {
        if (!writeSettings()) {
            return {};
        }
    }

    std::optional<RoomEvent> event{};
    if (!read(event)) {
        return {};
    }

    if (!event) {
        return State::Main;
    }

    switch (event->which_event) {
    case RoomEvent_join_tag:
        if (event->event.join.mii.size != sizeof(System::RawMii)) {
            return {};
        } else {
            auto *mii = reinterpret_cast<System::RawMii *>(event->event.join.mii.bytes);
            u32 location = event->event.join.location;
            u16 latitude = event->event.join.latitude;
            u16 longitude = event->event.join.longitude;
            u32 regionLineColor = event->event.join.regionLineColor;
            if (!onPlayerJoin(handler, mii, location, latitude, longitude, regionLineColor)) {
                return {};
            }
        }
        return State::Main;
    case RoomEvent_leave_tag:
        if (!onPlayerLeave(handler, event->event.leave.playerId)) {
            return {};
        }
        return State::Main;
    case RoomEvent_comment_tag:
        if (!onReceiveComment(handler, event->event.comment.playerId,
                    event->event.comment.messageId)) {
            return {};
        }
        return State::Main;
    case RoomEvent_settings_tag:
        if (event->event.settings.settings_count != RoomSettings::count) {
            return {};
        } else {
            bool changed = false;
            for (size_t i = 0; i < RoomSettings::count; i++) {
                changed = changed ||
                        event->event.settings.settings[i] !=
                                m_players[m_localPlayerIds[0]].m_settings[i];
                m_players[m_localPlayerIds[0]].m_settings[i] = event->event.settings.settings[i];
            }
            if (changed) {
                handler.onSettingsChange(m_players[m_localPlayerIds[0]].m_settings);
            }
        }
        return State::Main;
    case RoomEvent_close_tag:
        if (!onRoomClose(handler, event->event.close.gamemode)) {
            return {};
        }
        return State::Select;
    default:
        return State::Main;
    }
}

std::optional<RoomClient::State> RoomClient::calcSelect(Handler &handler) {
    std::optional<RoomEvent> event{};
    if (!read(event)) {
        return {};
    }

    if (!event) {
        return State::Select;
    }

    switch (event->which_event) {
    case RoomEvent_selectPulse_tag:
        if (!onReceivePulse(handler, event->event.selectPulse.playerId)) {
            return {};
        }
        return State::Select;
    case RoomEvent_selectInfo_tag:
        if (!onReceiveInfo(handler, *event)) {
            return {};
        }
        return State::Select;
    default:
        return State::Select;
    }
}

bool RoomClient::onSetup(Handler &handler) {
    handler.onSetup();

    return writeJoin();
}

bool RoomClient::onMain(Handler &handler) {
    handler.onMain();

    for (size_t i = 0; i < m_localPlayerCount; i++) {
        m_localPlayerIds[i] = m_playerCount;
        auto *globalContext = UI::SectionManager::Instance()->globalContext();
        System::Mii *mii = globalContext->m_localPlayerMiis.get(i);
        assert(mii);
        System::RawMii raw = mii->id()->getRaw();
        auto *saveManager = System::SaveManager::Instance();
        u32 location;
        saveManager->getLocation(&location);
        u16 latitude;
        saveManager->getLatitude(&latitude);
        u16 longitude;
        saveManager->getLongitude(&longitude);
        u32 regionLineColor = static_cast<u32>(
                saveManager->getSetting<SP::ClientSettings::Setting::RegionLineColor>());
        if (!onPlayerJoin(handler, &raw, location, latitude, longitude, regionLineColor)) {
            // TODO spectate?
            return false;
        }
    }
    return true;
}

bool RoomClient::onPlayerJoin(Handler &handler, const System::RawMii *mii, u32 location,
        u16 latitude, u16 longitude, u32 regionLineColor) {
    if (m_playerCount == 12) {
        return false;
    }

    m_players[m_playerCount].m_mii = *mii;
    m_playerCount++;
    handler.onPlayerJoin(mii, location, latitude, longitude, regionLineColor);
    return true;
}

bool RoomClient::onPlayerLeave(Handler &handler, u32 playerId) {
    if (playerId >= m_playerCount) {
        return false;
    }
    for (size_t i = 0; i < m_localPlayerCount; i++) {
        if (playerId == m_localPlayerIds[i]) {
            return false;
        }
    }
    m_playerCount--;
    handler.onPlayerLeave(playerId);
    return true;
}

bool RoomClient::onReceiveComment(Handler &handler, u32 playerId, u32 messageId) {
    if (playerId >= m_playerCount) {
        return false;
    }
    if (messageId >= 96) {
        return false;
    }
    handler.onReceiveComment(playerId, messageId);
    return true;
}

bool RoomClient::onRoomClose(Handler &handler, u32 gamemode) {
    if (gamemode >= 4) {
        return false;
    }
    handler.onRoomClose(gamemode);
    return true;
}

bool RoomClient::onReceivePulse(Handler &handler, u32 playerId) {
    if (playerId >= m_playerCount) {
        return false;
    }

    m_votePlayerOrder[playerId] = m_voteCurrentPlayerIdx;
    m_voteCurrentPlayerIdx++;
    handler.onReceivePulse(playerId);
    return true;
}

bool RoomClient::onReceiveInfo(Handler &handler, RoomEvent event) {
    for (u8 i = 0; i < event.event.selectInfo.playerProperties_count; i++) {
        RoomEvent_Properties &properties = event.event.selectInfo.playerProperties[i];
        m_players[i].m_course = properties.course;
        m_players[i].m_properties = {properties.character, properties.vehicle,
                properties.driftType};
        handler.onReceiveInfo(i, properties.course, event.event.selectInfo.selectedPlayer);
    }
    return true;
}

bool RoomClient::read(std::optional<RoomEvent> &event) {
    u8 buffer[RoomEvent_size];
    std::optional<u16> size = m_socket.read(buffer, sizeof(buffer));
    if (!size) {
        return false;
    }

    if (*size == 0) {
        return true;
    }

    pb_istream_t stream = pb_istream_from_buffer(buffer, *size);

    RoomEvent tmp;
    if (!pb_decode(&stream, RoomEvent_fields, &tmp)) {
        return false;
    }

    event = tmp;
    return true;
}

bool RoomClient::writeJoin() {
    RoomRequest request;
    request.which_request = RoomRequest_join_tag;
    request.request.join.miis_count = m_localPlayerCount;
    auto *globalContext = UI::SectionManager::Instance()->globalContext();
    for (size_t i = 0; i < m_localPlayerCount; i++) {
        System::Mii *mii = globalContext->m_localPlayerMiis.get(i);
        assert(mii);
        System::RawMii raw = mii->id()->getRaw();
        request.request.join.miis[i].size = sizeof(System::RawMii);
        memcpy(request.request.join.miis[i].bytes, &raw, sizeof(System::RawMii));
    }
    auto *saveManager = System::SaveManager::Instance();
    saveManager->getLocation(&request.request.join.location);
    u16 latitude;
    saveManager->getLatitude(&latitude);
    request.request.join.latitude = latitude;
    u16 longitude;
    saveManager->getLongitude(&longitude);
    request.request.join.longitude = longitude;
    request.request.join.regionLineColor = static_cast<u32>(
            saveManager->getSetting<SP::ClientSettings::Setting::RegionLineColor>());
    request.request.join.settings_count = RoomSettings::count;
    for (size_t i = 0; i < RoomSettings::count; i++) {
        request.request.join.settings[i] = saveManager->getSetting(RoomSettings::offset + i);
    }
    m_localSettingsChanged = false;
    return write(request);
}

bool RoomClient::writeComment(u32 messageId) {
    RoomRequest request;
    request.which_request = RoomRequest_comment_tag;
    request.request.comment.messageId = messageId;
    return write(request);
}

bool RoomClient::writeClose(u32 gamemode) {
    RoomRequest request;
    request.which_request = RoomRequest_close_tag;
    request.request.close.gamemode = gamemode;
    return write(request);
}

bool RoomClient::writeSettings() {
    RoomRequest request;
    request.which_request = RoomRequest_settings_tag;
    request.request.settings.settings_count = RoomSettings::count;
    auto *saveManager = System::SaveManager::Instance();
    for (size_t i = 0; i < RoomSettings::count; i++) {
        request.request.settings.settings[i] = saveManager->getSetting(RoomSettings::offset + i);
    }
    m_localSettingsChanged = false;
    return write(request);
}

bool RoomClient::writeVote(u32 course, std::optional<Player::Properties> properties) {
    RoomRequest request;
    request.which_request = RoomRequest_vote_tag;
    request.request.vote.course = course;
    if (properties) {
        request.request.vote.properties = {(*properties).m_character, (*properties).m_vehicle,
                (*properties).m_driftIsAuto};
    }
    return write(request);
}

bool RoomClient::write(RoomRequest request) {
    u8 buffer[RoomRequest_size];
    pb_ostream_t stream = pb_ostream_from_buffer(buffer, sizeof(buffer));

    assert(pb_encode(&stream, RoomRequest_fields, &request));

    return m_socket.write(buffer, stream.bytes_written);
}

RoomClient *RoomClient::s_instance = nullptr;

} // namespace SP
