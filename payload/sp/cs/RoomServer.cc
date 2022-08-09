#include "RoomServer.hh"

#include <egg/core/eggHeap.hh>
extern "C" {
#include <game/system/RootScene.h>
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
        for (u32 i = 0; i < m_playerCount; i++) {
            if (m_players[i].clientId == *m_disconnectQueue.front()) {
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

void RoomServer::OnCreateScene() {
    auto *sectionManager = UI::SectionManager::Instance();
    if (!sectionManager) {
        return;
    }

    switch (sectionManager->nextSectionId()) {
    case UI::SectionId::OnlineServer: {
        assert(!s_block);
        auto *heap = reinterpret_cast<EGG::Heap *>(s_rootScene->heapCollection.heaps[HEAP_ID_MEM2]);
        s_block = heap->alloc(sizeof(RoomServer), 0x4);
        break;
    }
    default:
        break;
    }
}

void RoomServer::OnDestroyScene() {
    auto *sectionManager = UI::SectionManager::Instance();
    if (!sectionManager) {
        return;
    }

    switch (sectionManager->lastSectionId()) {
    case UI::SectionId::OnlineServer: {
        if (s_instance) {
            DestroyInstance();
        }
        assert(s_block);
        auto *heap = reinterpret_cast<EGG::Heap *>(s_rootScene->heapCollection.heaps[HEAP_ID_MEM2]);
        heap->free(s_block);
        s_block = nullptr;
        break;
    }
    default:
        break;
    }
}

RoomServer *RoomServer::CreateInstance() {
    assert(s_block);
    assert(!s_instance);
    s_instance = new (s_block) RoomServer;
    return s_instance;
}

void RoomServer::DestroyInstance() {
    assert(s_instance);
    s_instance->~RoomServer();
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

bool RoomServer::onMain(Handler &handler) {
    handler.onMain();
    return true;
}

bool RoomServer::onPlayerJoin(Handler &handler, u32 clientId, const System::RawMii *mii,
        u32 location, u16 latitude, u16 longitude) {
    if (m_playerCount == 12) {
        return false;
    }

    m_players[m_playerCount] = { clientId, *mii, location, latitude, longitude };
    m_playerCount++;
    writeJoin(mii, location, latitude, longitude);
    handler.onPlayerJoin(mii, location, latitude, longitude);
    return true;
}

void RoomServer::onPlayerLeave(Handler &handler, u32 playerId) {
    handler.onPlayerLeave(playerId);
    writeLeave(playerId);
    m_playerCount--;
    for (u32 i = playerId; i < m_playerCount; i++) {
        m_players[i] = m_players[i + 1];
    }
}

void RoomServer::disconnectClient(u32 clientId) {
    m_clients[clientId].reset();
    m_disconnectQueue.push(std::move(clientId));
}

void RoomServer::writeJoin(const System::RawMii *mii, u32 location, u32 latitude, u32 longitude) {
    for (size_t i = 0; i < m_clients.size(); i++) {
        if (m_clients[i] && m_clients[i]->ready()) {
            if (!m_clients[i]->writeJoin(mii, location, latitude, longitude)) {
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

RoomServer::Client::Client(RoomServer &server, u32 id, s32 handle,
        const hydro_kx_keypair &serverKeypair) : m_id(id), m_server(server),
        m_state(State::Connect), m_socket(handle, serverKeypair, "room    ") {}

RoomServer::Client::~Client() = default;

bool RoomServer::Client::ready() const {
    switch (m_state) {
    case State::Main:
        return true;
    default:
        return false;
    }
}

bool RoomServer::Client::calc(Handler &handler) {
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

bool RoomServer::Client::writeJoin(const System::RawMii *mii, u32 location, u32 latitude,
        u32 longitude) {
    RoomEvent event;
    event.which_event = RoomEvent_join_tag;
    event.event.join.mii.size = sizeof(System::RawMii);
    memcpy(event.event.join.mii.bytes, mii, sizeof(System::RawMii));
    event.event.join.location = location;
    event.event.join.latitude = latitude;
    event.event.join.longitude = longitude;
    return write(event);
}

bool RoomServer::Client::writeLeave(u32 playerId) {
    RoomEvent event;
    event.which_event = RoomEvent_leave_tag;
    event.event.leave.playerId = playerId;
    return write(event);
}

bool RoomServer::Client::writeHost(u32 playerId) {
    RoomEvent event;
    event.which_event = RoomEvent_host_tag;
    event.event.host.playerId = playerId;
    return write(event);
}

std::optional<RoomServer::Client::State> RoomServer::Client::resolve(Handler &handler) {
    switch (m_state) {
    case State::Connect:
        return calcConnect();
    case State::Setup:
        return calcSetup(handler);
    case State::Main:
        break;
    }

    return m_state;
}

bool RoomServer::Client::transition(Handler &handler, State state) {
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
        for (size_t i = 0; i < request->request.join.miis_count; i++) {
            auto *mii = reinterpret_cast<System::RawMii *>(request->request.join.miis[i].bytes);
            u32 location = request->request.join.location;
            u32 latitude = request->request.join.latitude;
            u32 longitude = request->request.join.longitude;
            if (!m_server.onPlayerJoin(handler, m_id, mii, location, latitude, longitude)) {
                // TODO spectate?
                return {};
            }
        }
        for (size_t i = 0; i < m_server.m_playerCount - request->request.join.miis_count; i++) {
            const auto &player = m_server.m_players[i];
            if (!writeJoin(&player.mii, player.location, player.latitude, player.longitude)) {
                return {};
            }
        }
        if (!writeHost(0)) { // TODO fix
            return {};
        }
        return State::Main;
    default:
        return {};
    }
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

void *RoomServer::s_block = nullptr;
RoomServer *RoomServer::s_instance = nullptr;

} // namespace SP
