#include "RoomServer.hh"

#include <egg/core/eggHeap.hh>
extern "C" {
#include <game/system/RootScene.h>
#include <game/util/Registry.h>
}
#include <game/ui/SectionManager.hh>
#include <vendor/nanopb/pb_decode.h>
#include <vendor/nanopb/pb_encode.h>

namespace SP {

bool RoomServer::calc(Handler &handler) {
    if (auto state = resolve(handler)) {
        if (!transition(handler, *state)) {
            return false;
        }
    } else {
        return false;
    }

    if (!m_listener.poll()) {
        return false;
    }

    if (auto handle = m_listener.accept()) {
        size_t i;
        for (i = 0; i < m_clients.size(); i++) {
            if (!m_clients[i]) {
                m_clients[i].emplace(*this, i, *handle, m_keypair);
                break;
            }
        }
        if (i == m_clients.size()) {
            SP_LOG("No available slot for client");
            SOClose(*handle);
        }
    }

    for (size_t i = 0; i < m_clients.size(); i++) {
        if (m_clients[i]) {
            if (!m_clients[i]->calc(handler)) {
                disconnectClient(i);
            }
        }
    }

    while (!m_disconnectQueue.empty()) {
        for (u32 i = m_playerCount; i-- > 0;) {
            if (m_players[i].m_clientId == *m_disconnectQueue.front()) {
                onPlayerLeave(handler, i);
            }
        }
        m_disconnectQueue.pop();
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

RoomServer *RoomServer::CreateInstance() {
    assert(s_block);
    assert(!s_instance);
    s_instance = new (s_block) RoomServer;
    RoomManager::s_instance = s_instance;
    return s_instance;
}

void RoomServer::DestroyInstance() {
    assert(s_instance);
    s_instance->~RoomServer();
    RoomManager::s_instance = nullptr;
    s_instance = nullptr;
}

RoomServer *RoomServer::Instance() {
    return s_instance;
}

RoomServer::RoomServer() : m_state(ServerState::Setup), m_listener(21330) {
    hydro_kx_keygen(&m_keypair);
}

RoomServer::~RoomServer() {
    hydro_memzero(&m_keypair, sizeof(m_keypair));
}

std::optional<RoomServer::ServerState> RoomServer::resolve(Handler &handler) {
    switch (m_state) {
    case ServerState::Setup:
        return calcSetup();
    case ServerState::Main:
        return calcMain(handler);
    case ServerState::Select:
        return calcSelect(handler);
    }

    return m_state;
}

bool RoomServer::transition(Handler &handler, ServerState state) {
    if (state == m_state) {
        return true;
    }

    bool result = true;
    switch (state) {
    case ServerState::Setup:
        break;
    case ServerState::Main:
        result = onMain(handler);
        break;
    case ServerState::Select:
        break;
    }
    m_state = state;
    return result;
}

std::optional<RoomServer::ServerState> RoomServer::calcSetup() {
    if (!m_listener.ready()) {
        return ServerState::Setup;
    }

    return ServerState::Main;
}

std::optional<RoomServer::ServerState> RoomServer::calcMain(Handler &handler) {
    ServerState state = ServerState::Main;

    if (m_commentTimer > 0) {
        m_commentTimer--;
    } else {
        if (!m_commentQueue.empty()) {
            const auto *comment = m_commentQueue.front();
            handler.onReceiveComment(comment->playerId, comment->messageId);
            writeComment(comment->playerId, comment->messageId);
            m_commentQueue.pop();
            m_commentTimer = 90;
        }
    }

    if (m_settingsChanged) {
        writeSettings();
        handler.onSettingsChange(m_players[0].m_settings);
        m_settingsChanged = false;
    }

    if (m_roomClosed) {
        assert(m_gamemode != -1);
        writeClose(m_gamemode);
        handler.onRoomClose(m_gamemode);
        m_roomClosed = false;
        state = ServerState::Select;
    }

    return state;
}

std::optional<RoomServer::ServerState> RoomServer::calcSelect(Handler &handler) {
    if (m_voteEvent) {
        for (u8 i = 0; i < 12; i++) {
            if (m_voted[i]) {
                m_voteCount++;
                writeSelectPulse(i);
                handler.onReceivePulse(i);
                m_voted[i] = false;
                break;
            }
        }
        m_voteEvent = false;
    }

    if (m_voteCount == m_playerCount) {
        if (m_voteDelay != 0) {
            m_voteDelay--;
            return ServerState::Select;
        }
        u32 selectedPlayer = hydro_random_uniform(m_playerCount);
        writeSelectInfo(selectedPlayer);
        // HACK: need this condition to reset
        m_voteCount = 0;
        // NOTE: We will use the server handler to transition to the race section here
    }

    return ServerState::Select;
}

bool RoomServer::onMain(Handler &handler) {
    handler.onMain();
    return true;
}

bool RoomServer::onPlayerJoin(Handler &handler, u32 clientId, const System::RawMii *mii,
        u32 location, u16 latitude, u16 longitude, u32 regionLineColor,
        const std::array<u32, RoomSettings::count> &settings) {
    if (m_playerCount == 12) {
        return false;
    }

    m_players[m_playerCount] = {clientId, settings, *mii, location, latitude, longitude,
            regionLineColor, 0xFFFFFFFF, {}};
    m_playerCount++;
    writeJoin(mii, location, latitude, longitude, regionLineColor);
    handler.onPlayerJoin(mii, location, latitude, longitude, regionLineColor);
    if (m_playerCount == 1) {
        handler.onSettingsChange(settings);
    }
    return true;
}

void RoomServer::onPlayerLeave(Handler &handler, u32 playerId) {
    handler.onPlayerLeave(playerId);
    writeLeave(playerId);
    auto settings = m_players[playerId].m_settings;
    m_playerCount--;
    for (u32 i = playerId; i < m_playerCount; i++) {
        m_players[i] = m_players[i + 1];
    }
    if (playerId == 0 && m_playerCount > 0) {
        bool changed = false;
        for (size_t i = 0; i < RoomSettings::count; i++) {
            changed = changed || m_players[0].m_settings[i] != settings[i];
        }
        if (changed) {
            writeSettings();
            handler.onSettingsChange(m_players[0].m_settings);
        }
    }
}

bool RoomServer::onReceiveComment(u32 playerId, u32 messageId) {
    if (playerId >= m_playerCount) {
        return false;
    }

    if (messageId >= 96) {
        return false;
    }

    if (m_commentQueue.full()) {
        return false;
    }

    m_commentQueue.push(Comment{playerId, messageId});
    return true;
}

bool RoomServer::onRoomClose(u32 playerId, s32 gamemode) {
    if (playerId != 0) {
        return false;
    }

    if (gamemode >= 4) {
        return false;
    }

    if (gamemode < 0) {
        return false;
    }

    m_gamemode = gamemode;
    m_commentQueue.reset();
    return true;
}

bool RoomServer::onReceiveVote(u32 playerId, u32 course,
        std::optional<Player::Properties> &properties) {
    if (playerId >= m_playerCount) {
        return false;
    }

    if (m_gamemode > 0) {
        // Stage validation
        if (course < 0x20 || course > 0x2f) {
            return false;
        }
    } else {
        // Course validation
        if (course > 0x1f) {
            return false;
        }
    }

    m_players[playerId].m_course = course;

    if (properties) {
        if (!validateProperties(playerId, *properties)) {
            return false;
        }
    }

    m_voted[playerId] = true;
    return true;
}

bool RoomServer::validateProperties(u32 playerId, Player::Properties &properties) {
    // Character validation
    if (properties.m_character >= 0x30) {
        return false;
    }
    if (properties.m_character == 0x1C || properties.m_character == 0x1D) {
        return false;
    }
    if (properties.m_character == 0x22 || properties.m_character == 0x23) {
        return false;
    }
    if (properties.m_character == 0x28 || properties.m_character == 0x29) {
        return false;
    }

    // Vehicle validation
    if (properties.m_vehicle >= 0x24) {
        return false;
    }
    if (getCharacterWeightClass(properties.m_character) !=
            getVehicleWeightClass(properties.m_vehicle)) {
        return false;
    }

    m_players[playerId].m_properties = properties;
    return true;
}

void RoomServer::disconnectClient(u32 clientId) {
    m_clients[clientId].reset();
    m_disconnectQueue.push(std::move(clientId));
}

void RoomServer::writeJoin(const System::RawMii *mii, u32 location, u32 latitude, u32 longitude,
        u32 regionLineColor) {
    for (size_t i = 0; i < m_clients.size(); i++) {
        if (m_clients[i] && m_clients[i]->ready()) {
            if (!m_clients[i]->writeJoin(mii, location, latitude, longitude, regionLineColor)) {
                disconnectClient(i);
            }
        }
    }
}

void RoomServer::writeLeave(u32 playerId) {
    for (size_t i = 0; i < m_clients.size(); i++) {
        if (m_clients[i] && m_clients[i]->ready()) {
            if (!m_clients[i]->writeLeave(playerId)) {
                disconnectClient(i);
            }
        }
    }
}

void RoomServer::writeComment(u32 playerId, u32 messageId) {
    for (size_t i = 0; i < m_clients.size(); i++) {
        if (m_clients[i] && m_clients[i]->ready()) {
            if (!m_clients[i]->writeComment(playerId, messageId)) {
                disconnectClient(i);
            }
        }
    }
}

void RoomServer::writeSettings() {
    for (size_t i = 0; i < m_clients.size(); i++) {
        if (m_clients[i] && m_clients[i]->ready()) {
            if (!m_clients[i]->writeSettings()) {
                disconnectClient(i);
            }
        }
    }
}

void RoomServer::writeClose(u32 gamemode) {
    for (size_t i = 0; i < m_clients.size(); i++) {
        if (m_clients[i] && m_clients[i]->ready()) {
            if (!m_clients[i]->writeClose(gamemode)) {
                disconnectClient(i);
            }
        }
    }
}

void RoomServer::writeSelectPulse(u32 playerId) {
    for (size_t i = 0; i < m_clients.size(); i++) {
        if (m_clients[i] && m_clients[i]->ready()) {
            if (!m_clients[i]->writeSelectPulse(playerId)) {
                disconnectClient(i);
            }
        }
    }
}

void RoomServer::writeSelectInfo(u32 selectedPlayer) {
    for (size_t i = 0; i < m_clients.size(); i++) {
        if (m_clients[i] && m_clients[i]->ready()) {
            if (!m_clients[i]->writeSelectInfo(selectedPlayer)) {
                disconnectClient(i);
            }
        }
    }
}

RoomServer::Client::Client(RoomServer &server, u32 id, s32 handle,
        const hydro_kx_keypair &serverKeypair)
    : m_id(id), m_server(server), m_state(ClientState::Connect),
      m_socket(handle, serverKeypair, "room    ") {}

RoomServer::Client::~Client() = default;

bool RoomServer::Client::ready() const {
    switch (m_state) {
    case ClientState::Main:
    case ClientState::Select:
        return true;
    default:
        return false;
    }
}

bool RoomServer::Client::calc(Handler &handler) {
    if (auto state = resolve(handler)) {
        if (!transition(handler, *state)) {
            SP_LOG("Client::calc: First !transition");
            return false;
        }
    } else {
        SP_LOG("Client::calc: First !resolve");
        return false;
    }

    if (!m_socket.poll()) {
        SP_LOG("Client::calc: !m_socket.poll");
        return false;
    }

    if (auto state = resolve(handler)) {
        if (!transition(handler, *state)) {
            SP_LOG("Client::calc: Second !transition");
            return false;
        }
    } else {
        SP_LOG("Client::calc: Second !resolve");
        return false;
    }

    return true;
}

bool RoomServer::Client::writeJoin(const System::RawMii *mii, u32 location, u32 latitude,
        u32 longitude, u32 regionLineColor) {
    RoomEvent event;
    event.which_event = RoomEvent_join_tag;
    event.event.join.mii.size = sizeof(System::RawMii);
    memcpy(event.event.join.mii.bytes, mii, sizeof(System::RawMii));
    event.event.join.location = location;
    event.event.join.latitude = latitude;
    event.event.join.longitude = longitude;
    event.event.join.regionLineColor = regionLineColor;
    return write(event);
}

bool RoomServer::Client::writeLeave(u32 playerId) {
    RoomEvent event;
    event.which_event = RoomEvent_leave_tag;
    event.event.leave.playerId = playerId;
    return write(event);
}

bool RoomServer::Client::writeSettings() {
    RoomEvent event;
    event.which_event = RoomEvent_settings_tag;
    if (m_server.m_playerCount == 0) {
        event.event.settings.settings_count = 0;
    } else {
        const Player &host = m_server.m_players[0];
        assert(std::size(event.event.settings.settings) == std::size(host.m_settings));
        event.event.settings.settings_count = std::size(host.m_settings);
        for (size_t i = 0; i < std::size(host.m_settings); i++) {
            event.event.settings.settings[i] = host.m_settings[i];
        }
    }
    return write(event);
}

bool RoomServer::Client::writeComment(u32 playerId, u32 messageId) {
    RoomEvent event;
    event.which_event = RoomEvent_comment_tag;
    event.event.comment.playerId = playerId;
    event.event.comment.messageId = messageId;
    return write(event);
}

bool RoomServer::Client::writeClose(u32 gamemode) {
    RoomEvent event;
    event.which_event = RoomEvent_close_tag;
    event.event.close.gamemode = gamemode;
    return write(event);
}

bool RoomServer::Client::writeSelectPulse(u32 playerId) {
    RoomEvent event;
    event.which_event = RoomEvent_selectPulse_tag;
    event.event.selectPulse.playerId = playerId;
    return write(event);
}

bool RoomServer::Client::writeSelectInfo(u32 selectedPlayer) {
    RoomEvent event;
    event.which_event = RoomEvent_selectInfo_tag;
    event.event.selectInfo.playerProperties_count = m_server.m_playerCount;
    for (u8 i = 0; i < m_server.m_playerCount; i++) {
        const Player &player = m_server.m_players[i];
        event.event.selectInfo.playerProperties[i] = {player.m_properties.m_character,
                player.m_properties.m_vehicle, player.m_properties.m_driftIsAuto, player.m_course};
    }
    event.event.selectInfo.selectedPlayer = selectedPlayer;
    return write(event);
}

std::optional<RoomServer::ClientState> RoomServer::Client::resolve(Handler &handler) {
    switch (m_state) {
    case ClientState::Connect:
        return calcConnect();
    case ClientState::Setup:
        return calcSetup(handler);
    case ClientState::Main:
        return calcMain(handler);
    case ClientState::Select:
        return calcSelect(handler);
    }

    return m_state;
}

bool RoomServer::Client::transition(Handler &handler, ClientState state) {
    if (state == m_state) {
        return true;
    }

    bool result = true;
    switch (state) {
    case ClientState::Connect:
        break;
    case ClientState::Setup:
        break;
    case ClientState::Main:
        break;
    case ClientState::Select:
        break;
    }
    m_state = state;
    return result;
}

std::optional<RoomServer::ClientState> RoomServer::Client::calcConnect() {
    if (!m_socket.ready()) {
        return ClientState::Connect;
    }

    return ClientState::Setup;
}

std::optional<RoomServer::ClientState> RoomServer::Client::calcSetup(Handler &handler) {
    std::optional<RoomRequest> request{};
    if (!read(request)) {
        return {};
    }

    if (!request) {
        return ClientState::Setup;
    }

    switch (request->which_request) {
    case RoomRequest_join_tag:
        if (request->request.join.miis_count < 1 || request->request.join.miis_count > 2) {
            return {};
        }
        for (size_t i = 0; i < request->request.join.miis_count; i++) {
            if (request->request.join.miis[i].size != sizeof(System::RawMii)) {
                return {};
            }
        }
        if (request->request.join.settings_count != RoomSettings::count) {
            return {};
        }
        std::array<u32, RoomSettings::count> settings;
        for (size_t i = 0; i < RoomSettings::count; i++) {
            settings[i] = request->request.join.settings[i];
        }
        for (size_t i = 0; i < request->request.join.miis_count; i++) {
            auto *mii = reinterpret_cast<System::RawMii *>(request->request.join.miis[i].bytes);
            u32 location = request->request.join.location;
            u32 latitude = request->request.join.latitude;
            u32 longitude = request->request.join.longitude;
            u32 regionLineColor = request->request.join.regionLineColor;
            if (!m_server.onPlayerJoin(handler, m_id, mii, location, latitude, longitude,
                        regionLineColor, settings)) {
                // TODO spectate?
                return {};
            }
        }
        for (size_t i = 0; i < m_server.m_playerCount - request->request.join.miis_count; i++) {
            const auto &player = m_server.m_players[i];
            if (!writeJoin(&player.m_mii, player.m_location, player.m_latitude, player.m_longitude,
                        player.m_regionLineColor)) {
                return {};
            }
        }
        if (!writeSettings()) {
            return {};
        }
        return ClientState::Main;
    default:
        return {};
    }
}

std::optional<RoomServer::ClientState> RoomServer::Client::calcMain(Handler &handler) {
    if (m_server.m_state == ServerState::Select) {
        return ClientState::Select;
    }

    auto playerId = getPlayerId();
    if (!playerId) {
        return {};
    }

    std::optional<RoomRequest> request{};
    if (!read(request)) {
        return {};
    }

    if (!request) {
        return ClientState::Main;
    }
    switch (request->which_request) {
    case RoomRequest_comment_tag:
        if (!m_server.onReceiveComment(*playerId, request->request.comment.messageId)) {
            return {};
        }
        return ClientState::Main;
    case RoomRequest_settings_tag:
        if (request->request.settings.settings_count != RoomSettings::count) {
            return {};
        }
        std::array<u32, RoomSettings::count> settings;
        for (size_t i = 0; i < RoomSettings::count; i++) {
            settings[i] = request->request.settings.settings[i];
        }
        if (*playerId == 0) {
            for (size_t i = 0; i < RoomSettings::count; i++) {
                m_server.m_settingsChanged = m_server.m_settingsChanged ||
                        m_server.m_players[*playerId].m_settings[i] != settings[i];
            }
        }
        m_server.m_players[*playerId].m_settings = settings;
        return ClientState::Main;
    case RoomRequest_close_tag:
        if (!m_server.onRoomClose(*playerId, request->request.close.gamemode)) {
            return {};
        }
        m_server.m_roomClosed = true;
        return ClientState::Main;
    default:
        return {};
    }
}

std::optional<RoomServer::ClientState> RoomServer::Client::calcSelect(Handler &handler) {
    auto playerId = getPlayerId();
    if (!playerId) {
        return {};
    }

    std::optional<RoomRequest> request{};
    if (!read(request)) {
        return {};
    }

    if (!request) {
        return ClientState::Select;
    }

    std::optional<Player::Properties> properties;
    switch (request->which_request) {
    case RoomRequest_vote_tag:
        if (request->request.vote.has_properties) {
            properties = {request->request.vote.properties.character,
                    request->request.vote.properties.vehicle,
                    request->request.vote.properties.driftType};
        }
        if (!m_server.onReceiveVote(*playerId, request->request.vote.course, properties)) {
            return {};
        }
        m_server.m_voteEvent = true;
        return ClientState::Select;
    default:
        return {};
    }
}

bool RoomServer::Client::isHost() const {
    return m_server.m_playerCount > 0 && m_server.m_players[0].m_clientId == m_id;
}

std::optional<u32> RoomServer::Client::getPlayerId() const {
    for (size_t i = 0; i < m_server.m_playerCount; i++) {
        if (m_server.m_players[i].m_clientId == m_id) {
            return i;
        }
    }
    return {};
}

bool RoomServer::Client::read(std::optional<RoomRequest> &request) {
    u8 buffer[RoomRequest_size];
    std::optional<u16> size = m_socket.read(buffer, sizeof(buffer));
    if (!size) {
        return false;
    }

    if (*size == 0) {
        return true;
    }

    pb_istream_t stream = pb_istream_from_buffer(buffer, *size);

    RoomRequest tmp;
    if (!pb_decode(&stream, RoomRequest_fields, &tmp)) {
        return false;
    }

    request = tmp;
    return true;
}

bool RoomServer::Client::write(RoomEvent event) {
    u8 buffer[RoomEvent_size];
    pb_ostream_t stream = pb_ostream_from_buffer(buffer, sizeof(buffer));

    assert(pb_encode(&stream, RoomEvent_fields, &event));

    return m_socket.write(buffer, stream.bytes_written);
}

RoomServer *RoomServer::s_instance = nullptr;

} // namespace SP
