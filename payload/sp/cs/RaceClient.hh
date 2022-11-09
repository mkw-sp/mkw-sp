#pragma once

#include "sp/cs/RaceManager.hh"

namespace SP {

class RaceClient final : public RaceManager {
public:
    void destroyInstance() override;

    static RaceClient *CreateInstance(hydro_kx_session_keypair keypair, u32 ip, u16 port);
    static void DestroyInstance();
    static RaceClient *Instance();

private:
    RaceClient(hydro_kx_session_keypair keypair, u32 ip, u16 port);
    ~RaceClient();

    Net::UnreliableSocket m_socket;
    static RaceClient *s_instance;
};

} // namespace SP
