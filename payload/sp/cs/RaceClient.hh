#pragma once

#include "sp/CircularBuffer.hh"
#include "sp/cs/RaceManager.hh"

#include <protobuf/Race.pb.h>

namespace SP {

class RaceClient final : public RaceManager {
public:
    void destroyInstance() override;

    s32 drift() const;
    void adjustDrift();

    void calcWrite();
    void calcRead();

    static RaceClient *CreateInstance();
    static void DestroyInstance();
    static RaceClient *Instance();

private:
    struct ConnectionGroup : public Net::UnreliableSocket::ConnectionGroup {
    public:
        ConnectionGroup(RaceClient &client);

        u32 count() override;
        Net::UnreliableSocket::Connection &operator[](u32 index) override;

    private:
        RaceClient &m_client;
    };

    RaceClient(u32 ip, u16 port, hydro_kx_session_keypair keypair);
    ~RaceClient();

    s32 m_drift = 0;
    CircularBuffer<s32, 60> m_drifts;
    std::optional<RaceServerFrame> m_frame{};
    Net::UnreliableSocket::Connection m_connection;
    Net::UnreliableSocket m_socket;

    static RaceClient *s_instance;
};

} // namespace SP
