#pragma once

#include "sp/cs/RaceManager.hh"

#include <protobuf/Race.pb.h>

namespace SP {

class RaceServer final : public RaceManager {
public:
    void destroyInstance() override;

    void calcRead();

    static RaceServer *CreateInstance();
    static void DestroyInstance();
    static RaceServer *Instance();

private:
    struct Client {
        std::optional<RaceClientFrame> frame;
        Net::UnreliableSocket::Connection connection;
    };

    RaceServer(std::array<std::optional<Client>, 12> clients, u16 port);
    ~RaceServer();

    std::array<std::optional<Client>, 12> m_clients{};
    Net::UnreliableSocket m_socket;
    static RaceServer *s_instance;
};

} // namespace SP
