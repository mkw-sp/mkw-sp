#pragma once

#include "sp/cs/RaceManager.hh"

#include <protobuf/Race.pb.h>

namespace SP {

class RaceClient final : public RaceManager {
public:
    void destroyInstance() override;

    void calcWrite();

    static RaceClient *CreateInstance();
    static void DestroyInstance();
    static RaceClient *Instance();

private:
    RaceClient(u32 ip, u16 port, hydro_kx_session_keypair keypair);
    ~RaceClient();

    void write(RaceClientFrame frame);

    u32 m_frameId = 0;
    Net::UnreliableSocket::Connection m_connection;
    Net::UnreliableSocket m_socket;

    static RaceClient *s_instance;
};

} // namespace SP
