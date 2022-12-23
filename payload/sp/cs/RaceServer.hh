#pragma once

#include "sp/cs/RaceManager.hh"

#include <protobuf/Race.pb.h>

namespace SP {

class RaceServer final : public RaceManager {
public:
    void destroyInstance() override;

    void calcRead();
    void calcWrite();

    static RaceServer *CreateInstance();
    static void DestroyInstance();
    static RaceServer *Instance();

private:
    struct Client {
        std::optional<RaceClientFrame> frame;
        Net::UnreliableSocket::Connection connection;
    };

    struct ConnectionGroup : public Net::UnreliableSocket::ConnectionGroup {
    public:
        ConnectionGroup(RaceServer &server);

        u32 clientId(u32 index) const;

        u32 count() override;
        Net::UnreliableSocket::Connection &operator[](u32 index) override;

    private:
        RaceServer &m_server;
        u32 m_clientCount = 0;
        std::array<u32, 12> m_clientIds{};
    };

    RaceServer(std::array<std::optional<Client>, 12> clients, u16 port);
    ~RaceServer();

    bool isFrameValid(const RaceClientFrame &frame, u32 clientId);

    std::array<std::optional<Client>, 12> m_clients{};
    Net::UnreliableSocket m_socket;
    static RaceServer *s_instance;
};

} // namespace SP
