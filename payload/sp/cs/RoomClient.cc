#include "RoomClient.hh"

#include "sp/settings/RegionLineColor.hh"

#include <game/system/SaveManager.hh>
#include <game/ui/GlobalContext.hh>
#include <game/ui/SectionManager.hh>

namespace SP {

bool RoomClient::isPlayerLocal(u32 playerId) const {
    for (size_t i = 0; i < m_localPlayerCount; i++) {
        if (m_localPlayerIds[i] == playerId) {
            return true;
        }
    }

    return false;
}

bool RoomClient::isPlayerRemote(u32 playerId) const {
    return playerId < m_playerCount && !isPlayerLocal(playerId);
}

bool RoomClient::canSelectTeam(u32 playerId) const {
    return isPlayerLocal(playerId);
}

bool RoomClient::canSelectTeam(u32 localPlayerId, u32 playerId) const {
    assert(localPlayerId < m_localPlayerCount);
    return m_localPlayerIds[localPlayerId] == playerId;
}

void RoomClient::destroyInstance() {
    DestroyInstance();
}

bool RoomClient::calc(Handler &handler) {
    if (m_errorMessage != nullptr) {
        if (!m_errored) {
            handler.onError(m_errorMessage);
            m_errored = true;
        }

        return false;
    }

    if (auto state = resolve(handler)) {
        transition(handler, *state);
    } else {
        return false;
    }

    if (!m_socket.inner().poll()) {
        handler.onError(nullptr);
    }

    if (auto state = resolve(handler)) {
        transition(handler, *state);
    } else {
        return false;
    }

    return true;
}

u32 RoomClient::ip() const {
    return m_ip;
}

u16 RoomClient::port() const {
    return m_port;
}

hydro_kx_session_keypair RoomClient::keypair() const {
    return m_socket.inner().keypair();
}

Net::AsyncSocket &RoomClient::socket() {
    return m_socket.inner();
}

void RoomClient::sendComment(u32 commentId) {
    auto res = writeComment(commentId);
    if (!res && m_errorMessage != nullptr) {
        m_errorMessage = res.error();
    }
}

void RoomClient::startRoom(u32 gamemode) {
    auto res = writeStart(gamemode);
    if (!res && m_errorMessage != nullptr) {
        m_errorMessage = res.error();
    }
}

void RoomClient::changeLocalSettings() {
    m_localSettingsChanged = true;
}

void RoomClient::sendTeamSelect(u32 playerId) {
    m_players[playerId].m_teamId = (m_players[playerId].m_teamId + 1) % 6;
    auto res = writeTeamSelect(playerId, m_players[playerId].m_teamId);
    if (!res && m_errorMessage != nullptr) {
        m_errorMessage = res.error();
    }
}

void RoomClient::sendVote(Registry::Course course, std::optional<Player::Properties> properties) {
    auto res = writeVote(static_cast<u32>(course), properties);
    if (!res && m_errorMessage != nullptr) {
        m_errorMessage = res.error();
    }
}

RoomClient *RoomClient::CreateInstance(u32 localPlayerCount, u32 ip, u16 port, u16 passcode) {
    assert(s_block);
    assert(!s_instance);
    s_instance = new (s_block) RoomClient(localPlayerCount, ip, port, passcode);
    RoomManager::s_instance = s_instance;
    return s_instance;
}

RoomClient *RoomClient::CreateInstance(u32 localPlayerCount, u32 ip, u16 port,
        LoginInfo loginInfo) {
    assert(s_block);
    assert(!s_instance);
    s_instance = new (s_block) RoomClient(localPlayerCount, ip, port, loginInfo);
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

RoomClient::RoomClient(u32 localPlayerCount, u32 ip, u16 port, u16 passcode)
    : m_localPlayerCount(localPlayerCount), m_state(State::Connect),
      m_innerSocket(ip, port, "room    "),
      m_socket(&m_innerSocket, RoomEvent_fields, RoomRequest_fields), m_ip(ip), m_port(port) {
    m_passcode = passcode;
}

RoomClient::RoomClient(u32 localPlayerCount, u32 ip, u16 port, LoginInfo loginInfo)
    : m_localPlayerCount(localPlayerCount), m_state(State::Connect),
      m_innerSocket(ip, port, "room    "),
      m_socket(&m_innerSocket, RoomEvent_fields, RoomRequest_fields), m_ip(ip), m_port(port) {
    m_loginInfo = loginInfo;
}

RoomClient::~RoomClient() = default;

std::optional<RoomClient::State> RoomClient::resolve(Handler &handler) {
    std::expected<std::optional<RoomClient::State>, const wchar_t *> res;
    switch (m_state) {
    case State::Connect:
        res = calcConnect();
        break;
    case State::Setup:
        res = calcSetup(handler);
        break;
    case State::Main:
        res = calcMain(handler);
        break;
    case State::TeamSelect:
        res = calcTeamSelect(handler);
        break;
    case State::Select:
        res = calcSelect(handler);
        break;
    case State::Race:
        return m_state;
    }

    if (res) {
        return *res;
    } else {
        m_errorMessage = res.error();
        return {};
    }
}

void RoomClient::transition(Handler &handler, State state) {
    if (state == m_state) {
        return;
    }

    std::expected<void, const wchar_t *> result = {};
    switch (state) {
    case State::Connect:
        break;
    case State::Setup:
        result = onSetup(handler);
        break;
    case State::Main:
        onMain(handler);
        break;
    case State::TeamSelect:
        onTeamSelect(handler);
        break;
    case State::Select:
        onSelect(handler);
        break;
    case State::Race:
        break;
    }

    if (result) {
        m_state = state;
    } else {
        m_errorMessage = result.error();
    }
}

std::expected<RoomClient::State, const wchar_t *> RoomClient::calcConnect() {
    if (!m_socket.inner().ready()) {
        return State::Connect;
    }

    return State::Setup;
}

std::expected<RoomClient::State, const wchar_t *> RoomClient::calcSetup(Handler &handler) {
    std::optional<RoomEvent> event = TRY(m_socket.readProto());
    if (!event) {
        return State::Setup;
    }

    switch (event->which_event) {
    case RoomEvent_join_tag:
        assert(event->event.join.mii.size == sizeof(System::RawMii));
        {
            auto *mii = reinterpret_cast<System::RawMii *>(event->event.join.mii.bytes);
            u32 location = event->event.join.location;
            u16 latitude = event->event.join.latitude;
            u16 longitude = event->event.join.longitude;
            u32 regionLineColor = event->event.join.regionLineColor;
            onPlayerJoin(handler, mii, location, latitude, longitude, regionLineColor);
        }
        return State::Setup;
    case RoomEvent_settings_tag:
        if (m_playerCount == 0) {
            auto *saveManager = System::SaveManager::Instance();
            for (size_t i = 0; i < RoomSettings::count; i++) {
                m_settings[i] = saveManager->getSetting(RoomSettings::offset + i);
            }
        } else {
            assert(event->event.settings.settings_count == RoomSettings::count);
            for (size_t i = 0; i < RoomSettings::count; i++) {
                const auto &entry = SP::ClientSettings::entries[SP::RoomSettings::offset + i];
                assert(event->event.settings.settings[i] < entry.valueCount);
            }
            for (size_t i = 0; i < RoomSettings::count; i++) {
                m_settings[i] = event->event.settings.settings[i];
            }
        }
        handler.onSettingsChange(m_settings);
        return State::Main;
    default:
        return {};
    }
}

std::expected<RoomClient::State, const wchar_t *> RoomClient::calcMain(Handler &handler) {
    if (m_localSettingsChanged) {
        if (isPlayerLocal(0)) {
            TRY(writeSettings());
        }
        m_localSettingsChanged = false;
    }

    std::optional<RoomEvent> event = TRY(m_socket.readProto());
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
            onPlayerJoin(handler, mii, location, latitude, longitude, regionLineColor);
        }
        return State::Main;
    case RoomEvent_leave_tag:
        onPlayerLeave(handler, event->event.leave.playerId);
        return State::Main;
    case RoomEvent_comment_tag:
        onReceiveComment(handler, event->event.comment.playerId, event->event.comment.messageId);
        return State::Main;
    case RoomEvent_settings_tag:
        assert(event->event.settings.settings_count == RoomSettings::count);
        for (size_t i = 0; i < RoomSettings::count; i++) {
            const auto &entry = SP::ClientSettings::entries[SP::RoomSettings::offset + i];
            assert(event->event.settings.settings[i] < entry.valueCount);
        }
        {
            bool changed = false;
            for (size_t i = 0; i < RoomSettings::count; i++) {
                changed = changed || event->event.settings.settings[i] != m_settings[i];
                m_settings[i] = event->event.settings.settings[i];
            }
            if (changed) {
                handler.onSettingsChange(m_settings);
            }
        }
        return State::Main;
    case RoomEvent_start_tag:
        onRoomStart(handler, event->event.start.gamemode);
        {
            auto setting = getSetting<SP::ClientSettings::Setting::RoomTeamSize>();
            if (setting == SP::ClientSettings::TeamSize::FFA) {
                return State::Select;
            } else {
                return State::TeamSelect;
            }
        }
    default:
        return State::Main;
    }
}

std::expected<RoomClient::State, const wchar_t *> RoomClient::calcTeamSelect(Handler &handler) {
    std::optional<RoomEvent> event = TRY(m_socket.readProto());
    if (!event) {
        return State::TeamSelect;
    }

    switch (event->which_event) {
    case RoomEvent_teamSelect_tag:
        onReceiveTeamSelect(handler, event->event.teamSelect.playerId,
                event->event.teamSelect.teamId);
        return State::TeamSelect;
    default:
        return State::TeamSelect;
    }
}

std::expected<RoomClient::State, const wchar_t *> RoomClient::calcSelect(Handler &handler) {
    std::optional<RoomEvent> event = TRY(m_socket.readProto());
    if (!event) {
        return State::Select;
    }

    switch (event->which_event) {
    case RoomEvent_selectPulse_tag:
        onReceivePulse(handler, event->event.selectPulse.playerId);
        break;
    case RoomEvent_selectInfo_tag:
        onReceiveInfo(handler, *event);
        break;
    default:
        break;
    }

    return State::Select;
}

std::expected<void, const wchar_t *> RoomClient::onSetup(Handler &handler) {
    handler.onSetup();
    return writeJoin();
}

void RoomClient::onMain(Handler &handler) {
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
        SP::ClientSettings::RegionLineColor regionLineColorSetting =
                saveManager->getSetting<SP::ClientSettings::Setting::RegionLineColor>();
        u32 regionLineColor = SP::RegionLineColor::Get(regionLineColorSetting);
        // TODO spectate?
        onPlayerJoin(handler, &raw, location, latitude, longitude, regionLineColor);
    }
}

void RoomClient::onTeamSelect(Handler &handler) {
    handler.onTeamSelect();
}

void RoomClient::onSelect(Handler &handler) {
    handler.onSelect();
}

void RoomClient::onPlayerJoin(Handler &handler, const System::RawMii *mii, u32 location,
        u16 latitude, u16 longitude, u32 regionLineColor) {
    assert(m_playerCount != 12);

    m_players[m_playerCount] = {0xFFFFFFFF, *mii, location, latitude, longitude, regionLineColor,
            0xFFFFFFFF, {}, 0};
    m_playerCount++;
    handler.onPlayerJoin(mii, location, latitude, longitude, regionLineColor);
}

void RoomClient::onPlayerLeave(Handler &handler, u32 playerId) {
    assert(playerId < m_playerCount);
    assert(!isPlayerLocal(playerId));

    m_playerCount--;
    for (u32 i = playerId; i < m_playerCount; i++) {
        m_players[i] = m_players[i + 1];
    }
    for (u32 i = 0; i < m_localPlayerCount; i++) {
        if (m_localPlayerIds[i] > playerId) {
            m_localPlayerIds[i]--;
        }
    }
    handler.onPlayerLeave(playerId);
}

void RoomClient::onReceiveComment(Handler &handler, u32 playerId, u32 messageId) {
    assert(playerId < m_playerCount);
    assert(messageId < 96);

    handler.onReceiveComment(playerId, messageId);
}

void RoomClient::onRoomStart(Handler & /* handler */, u32 gamemode) {
    assert(gamemode < 3);
    m_gamemode = gamemode;
}

void RoomClient::onReceiveTeamSelect(Handler &handler, u32 playerId, u32 teamId) {
    assert(playerId < m_playerCount);
    assert(teamId < 6);

    handler.onReceiveTeamSelect(playerId, teamId);
}

void RoomClient::onReceivePulse(Handler &handler, u32 playerId) {
    assert(playerId < m_playerCount);

    m_votePlayerOrder[playerId] = m_voteCurrentPlayerIdx;
    m_voteCurrentPlayerIdx++;
    handler.onReceivePulse(playerId);
}

void RoomClient::onReceiveInfo(Handler &handler, RoomEvent event) {
    for (u8 i = 0; i < event.event.selectInfo.playerProperties_count; i++) {
        RoomEvent_Properties &properties = event.event.selectInfo.playerProperties[i];
        m_players[i].m_course = properties.course;
        m_players[i].m_properties = {properties.character, properties.vehicle,
                properties.driftType};
        handler.onReceiveInfo(i, static_cast<Registry::Course>(properties.course),
                event.event.selectInfo.selectedPlayer, properties.character, properties.vehicle);
    }
}

std::expected<void, const wchar_t *> RoomClient::writeJoin() {
    RoomRequest request;
    request.which_request = RoomRequest_join_tag;
    request.request.join.miis_count = m_localPlayerCount;
    if (m_loginInfo) {
        request.request.join.login_info = *m_loginInfo;
        request.request.join.has_login_info = true;
    } else {
        request.request.join.has_login_info = false;
    }

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
    SP::ClientSettings::RegionLineColor regionLineColorSetting =
            saveManager->getSetting<SP::ClientSettings::Setting::RegionLineColor>();
    request.request.join.regionLineColor = SP::RegionLineColor::Get(regionLineColorSetting);
    request.request.join.settings_count = RoomSettings::count;
    for (size_t i = 0; i < RoomSettings::count; i++) {
        request.request.join.settings[i] = saveManager->getSetting(RoomSettings::offset + i);
    }
    m_localSettingsChanged = false;
    return m_socket.writeProto(request);
}

std::expected<void, const wchar_t *> RoomClient::writeComment(u32 messageId) {
    RoomRequest request;
    request.which_request = RoomRequest_comment_tag;
    request.request.comment.messageId = messageId;
    return m_socket.writeProto(request);
}

std::expected<void, const wchar_t *> RoomClient::writeStart(u32 gamemode) {
    RoomRequest request;
    request.which_request = RoomRequest_start_tag;
    request.request.start.gamemode = gamemode;
    return m_socket.writeProto(request);
}

std::expected<void, const wchar_t *> RoomClient::writeSettings() {
    RoomRequest request;
    request.which_request = RoomRequest_settings_tag;
    request.request.settings.settings_count = RoomSettings::count;
    auto *saveManager = System::SaveManager::Instance();
    for (size_t i = 0; i < RoomSettings::count; i++) {
        request.request.settings.settings[i] = saveManager->getSetting(RoomSettings::offset + i);
    }
    return m_socket.writeProto(request);
}

std::expected<void, const wchar_t *> RoomClient::writeTeamSelect(u32 playerId, u32 teamId) {
    RoomRequest request;
    request.which_request = RoomRequest_teamSelect_tag;
    request.request.teamSelect.playerId = playerId;
    request.request.teamSelect.teamId = teamId;
    return m_socket.writeProto(request);
}

std::expected<void, const wchar_t *> RoomClient::writeVote(u32 course,
        std::optional<Player::Properties> properties) {
    RoomRequest request;
    request.which_request = RoomRequest_vote_tag;
    request.request.vote.course = course;
    if (properties) {
        request.request.vote.properties = {(*properties).m_character, (*properties).m_vehicle,
                (*properties).m_driftIsAuto};
    }
    return m_socket.writeProto(request);
}

RoomClient *RoomClient::s_instance = nullptr;

} // namespace SP
