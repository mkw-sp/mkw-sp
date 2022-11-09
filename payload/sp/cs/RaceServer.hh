#pragma once

#include "sp/cs/RaceManager.hh"

namespace SP {

class RaceServer final : public RaceManager {
public:
    void destroyInstance() override;

    static RaceServer *CreateInstance(
            std::array<Net::UnreliableSocket::ConnectionInfo, 24> &connections, u32 connectionCount,
            u16 port);
    static void DestroyInstance();
    static RaceServer *Instance();

private:
    RaceServer(std::array<Net::UnreliableSocket::ConnectionInfo, 24> &connections,
            u32 connectionCount, u16 port);
    ~RaceServer();

    Net::UnreliableSocket m_socket;
    static RaceServer *s_instance;
};

} // namespace SP
