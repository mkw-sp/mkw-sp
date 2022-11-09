#include "RaceClient.hh"

namespace SP {

void RaceClient::destroyInstance() {
    DestroyInstance();
}

RaceClient *RaceClient::CreateInstance(hydro_kx_session_keypair keypair, u32 ip, u16 port) {
    assert(s_block);
    assert(!s_instance);
    s_instance = new (s_block) RaceClient(keypair, ip, port);
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

RaceClient::RaceClient(hydro_kx_session_keypair keypair, u32 ip, u16 port) : m_socket({ip, port, keypair}, "race    ") {}

RaceClient::~RaceClient() = default;

RaceClient *RaceClient::s_instance = nullptr;

} // namespace SP
