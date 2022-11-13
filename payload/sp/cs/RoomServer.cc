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

bool RoomServer::isPlayerLocal([[maybe_unused]] u32 playerId) const {
    return false;
}

bool RoomServer::canSelectTeam([[maybe_unused]] u32 playerId) const {
    return false;
}

bool RoomServer::canSelectTeam([[maybe_unused]] u32 localPlayerId,
        [[maybe_unused]] u32 playerId) const {
    return false;
}

void RoomServer::destroyInstance() {
    DestroyInstance();
}

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

RoomServer::RoomServer() : m_state(State::Setup), m_listener(21330) {
    hydro_kx_keygen(&m_keypair);
}

RoomServer::~RoomServer() {
    hydro_memzero(&m_keypair, sizeof(m_keypair));
}

std::optional<RoomServer::State> RoomServer::resolve(Handler &handler) {
    switch (m_state) {
    case State::Setup:
        return calcSetup();
    case State::Main:
        return calcMain(handler);
    case State::TeamSelect:
        break;
    case State::Select:
        return calcSelect(handler);
    case State::Race:
        break;
    }

    return m_state;
}

bool RoomServer::transition(Handler &handler, State state) {
    if (state == m_state) {
        return true;
    }

    bool result = true;
    switch (state) {
    case State::Setup:
        break;
    case State::Main:
        result = onMain(handler);
        break;
    case State::TeamSelect:
        result = onTeamSelect(handler);
        break;
    case State::Select:
        result = onSelect(handler);
        break;
    case State::Race:
        // NOTE: We will use the server handler to transition to the race section here
        break;
    }
    m_state = state;
    return result;
}

std::optional<RoomServer::State> RoomServer::calcSetup() {
    if (!m_listener.ready()) {
        return State::Setup;
    }

    return State::Main;
}

std::optional<RoomServer::State> RoomServer::calcMain(Handler &handler) {
    State state = State::Main;

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
        handler.onSettingsChange(m_settings);
        m_settingsChanged = false;
    }

    if (m_roomStarted) {
        writeStart(m_gamemode);
        m_roomStarted = false;
        auto setting = getSetting<SP::ClientSettings::Setting::RoomTeamSize>();
        if (setting == SP::ClientSettings::RoomTeamSize::FFA) {
            state = State::Select;
        } else {
            state = State::TeamSelect;
        }
    }

    return state;
}

std::optional<RoomServer::State> RoomServer::calcSelect(Handler &handler) {
    if (m_voteEvent) {
        for (u8 i = 0; i < 12; i++) {
            if (m_voted[i]) {
                m_votePlayerOrder[i] = m_voteCurrentPlayerIdx;
                m_voteCurrentPlayerIdx++;

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
        u32 selectedPlayer = hydro_random_uniform(m_playerCount);
        writeSelectInfo(selectedPlayer);
        return State::Race;
    }

    return State::Select;
}

bool RoomServer::onMain(Handler &handler) {
    handler.onMain();
    return true;
}

bool RoomServer::onTeamSelect(Handler &handler) {
    handler.onTeamSelect();
    return true;
}

bool RoomServer::onSelect(Handler &handler) {
    handler.onSelect();
    return true;
}

bool RoomServer::onPlayerJoin(Handler &handler, u32 clientId, const System::RawMii *mii,
        u32 location, u16 latitude, u16 longitude, u32 regionLineColor,
        const std::array<u32, RoomSettings::count> &settings) {
    if (m_playerCount == 12) {
        return false;
    }

    m_players[m_playerCount] = {clientId, *mii, location, latitude, longitude,
            regionLineColor, 0xFFFFFFFF, {}, 0};
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
    m_playerCount--;
    for (u32 i = playerId; i < m_playerCount; i++) {
        m_players[i] = m_players[i + 1];
    }
    if (playerId == 0) {
        for (u32 i = 0; i < m_clients.size(); i++) {
            if (m_clients[i]) {
                disconnectClient(i);
            }
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

bool RoomServer::onRoomStart(u32 playerId, u32 gamemode) {
    if (playerId != 0) {
        return false;
    }

    if (gamemode >= 3) {
        return false;
    }

    m_gamemode = gamemode;
    m_commentQueue.reset();
    m_roomStarted = true;
    return true;
}

bool RoomServer::onReceiveTeamSelect(Handler &handler, u32 playerId, u32 teamId) {
    if (teamId >= 6) {
        return false;
    }

    m_players[playerId].m_teamId = teamId;
    handler.onReceiveTeamSelect(playerId, teamId);
    writeTeamSelect(playerId, teamId);
    return true;
}

bool RoomServer::onReceiveVote(u32 playerId, u32 course, Player::Properties &properties) {
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

    if (!validateProperties(playerId, properties)) {
        return false;
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

    // TODO: validate property changes depending on setting
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

void RoomServer::writeStart(u32 gamemode) {
    for (size_t i = 0; i < m_clients.size(); i++) {
        if (m_clients[i] && m_clients[i]->ready()) {
            if (!m_clients[i]->writeStart(gamemode)) {
                disconnectClient(i);
            }
        }
    }
}

void RoomServer::writeTeamSelect(u32 playerId, u32 teamId) {
    for (size_t i = 0; i < m_clients.size(); i++) {
        if (m_clients[i] && m_clients[i]->ready()) {
            if (!m_clients[i]->writeTeamSelect(playerId, teamId)) {
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
    : m_id(id), m_server(server), m_state(State::Connect),
      m_socket(handle, serverKeypair, "room    ") {}

RoomServer::Client::~Client() = default;

// TODO check for more precise state? or at least invert the check?
bool RoomServer::Client::ready() const {
    switch (m_state) {
    case State::Main:
    case State::TeamSelect:
    case State::Select:
    case State::Race:
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
        assert(std::size(event.event.settings.settings) == std::size(m_server.m_settings));
        event.event.settings.settings_count = std::size(m_server.m_settings);
        for (size_t i = 0; i < std::size(m_server.m_settings); i++) {
            event.event.settings.settings[i] = m_server.m_settings[i];
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

bool RoomServer::Client::writeStart(u32 gamemode) {
    RoomEvent event;
    event.which_event = RoomEvent_start_tag;
    event.event.start.gamemode = gamemode;
    return write(event);
}

bool RoomServer::Client::writeTeamSelect(u32 playerId, u32 teamId) {
    RoomEvent event;
    event.which_event = RoomEvent_teamSelect_tag;
    event.event.teamSelect.playerId = playerId;
    event.event.teamSelect.teamId = teamId;
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

std::optional<RoomServer::Client::State> RoomServer::Client::resolve(Handler &handler) {
    switch (m_state) {
    case State::Connect:
        return calcConnect();
    case State::Setup:
        return calcSetup(handler);
    case State::Main:
        return calcMain(handler);
    case State::TeamSelect:
        return calcTeamSelect(handler);
    case State::Select:
        return calcSelect(handler);
    case State::Race:
        break;
    }

    return m_state;
}

bool RoomServer::Client::transition(Handler &handler, ClientState state) {
    if (state == m_state) {
        return true;
    }

    bool result = true;
    switch (state) {
    case State::Connect:
        break;
    case State::Setup:
        break;
    case State::Main:
        break;
    case State::TeamSelect:
        break;
    case State::Select:
        break;
    case State::Race:
        break;
    }
    m_state = state;
    return result;
}

std::optional<RoomServer::Client::State> RoomServer::Client::calcConnect() {
    if (!m_socket.ready()) {
        return State::Connect;
    }

    return State::Setup;
}

std::optional<RoomServer::Client::State> RoomServer::Client::calcSetup(Handler &handler) {
    std::optional<RoomRequest> request{};
    if (!read(request)) {
        return {};
    }

    if (!request) {
        return State::Setup;
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
        for (size_t i = 0; i < RoomSettings::count; i++) {
            const auto &entry = SP::ClientSettings::entries[SP::RoomSettings::offset + i];
            if (request->request.join.settings[i] >= entry.valueCount) {
                return {};
            }
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
        return State::Main;
    default:
        return {};
    }
}

std::optional<RoomServer::Client::State> RoomServer::Client::calcMain(Handler &handler) {
    switch (m_server.m_state) {
    case RoomServer::State::TeamSelect:
        return State::TeamSelect;
    case RoomServer::State::Select:
        return State::Select;
    default:
        break;
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
        return State::Main;
    }
    switch (request->which_request) {
    case RoomRequest_comment_tag:
        if (!m_server.onReceiveComment(*playerId, request->request.comment.messageId)) {
            return {};
        }
        return State::Main;
    case RoomRequest_settings_tag:
        if (*playerId != 0) {
            return {};
        }
        if (request->request.settings.settings_count != RoomSettings::count) {
            return {};
        }
        for (size_t i = 0; i < RoomSettings::count; i++) {
            const auto &entry = SP::ClientSettings::entries[SP::RoomSettings::offset + i];
            if (request->request.settings.settings[i] >= entry.valueCount) {
                return {};
            }
        }
        std::array<u32, RoomSettings::count> settings;
        for (size_t i = 0; i < RoomSettings::count; i++) {
            settings[i] = request->request.settings.settings[i];
        }
        for (size_t i = 0; i < RoomSettings::count; i++) {
            if (m_server.m_settings[i] != settings[i]) {
                m_server.m_settingsChanged = true;
            }
        }
        m_server.m_settings = settings;
        return State::Main;
    case RoomRequest_start_tag:
        if (!m_server.onRoomStart(*playerId, request->request.start.gamemode)) {
            return {};
        }
        return State::Main;
    default:
        return {};
    }
}

std::optional<RoomServer::Client::State> RoomServer::Client::calcTeamSelect(Handler &handler) {
    auto playerId = getPlayerId();
    if (!playerId) {
        return {};
    }

    std::optional<RoomRequest> request{};
    if (!read(request)) {
        return {};
    }

    if (!request) {
        return State::TeamSelect;
    }

    switch (request->which_request) {
    case RoomRequest_teamSelect_tag:
        if (request->request.teamSelect.playerId > m_server.m_playerCount) {
            return {};
        }
        if (m_server.m_players[request->request.teamSelect.playerId].m_clientId != m_id) {
            return {};
        }
        if (!m_server.onReceiveTeamSelect(handler, request->request.teamSelect.playerId,
                    request->request.teamSelect.teamId)) {
            return {};
        }
        return State::TeamSelect;
    default:
        return {};
    }
}

std::optional<RoomServer::Client::State> RoomServer::Client::calcSelect(Handler &handler) {
    auto playerId = getPlayerId();
    if (!playerId) {
        return {};
    }

    std::optional<RoomRequest> request{};
    if (!read(request)) {
        return {};
    }

    if (!request) {
        return State::Select;
    }

    Player::Properties properties;
    switch (request->which_request) {
    case RoomRequest_vote_tag:
        properties = {request->request.vote.properties.character,
                request->request.vote.properties.vehicle,
                request->request.vote.properties.driftType};
        if (!m_server.onReceiveVote(*playerId, request->request.vote.course, properties)) {
            return {};
        }
        m_server.m_voteEvent = true;
        return State::Select;
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
