#pragma once

#include "sp/CircularBuffer.hh"
#include "sp/cs/RaceManager.hh"

namespace SP {

class RaceClient final : public RaceManager {
public:
    void destroyInstance() override;

    const std::optional<RaceServerFrame> &frame() const;
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

    bool isFrameValid(const RaceServerFrame &frame);

    static bool IsVec3Valid(const RaceServerFrame_Vec3 &v);
    static bool IsQuatValid(const RaceServerFrame_Quat &q);

    s32 m_drift = 0;
    CircularBuffer<s32, 60> m_drifts;
    std::optional<RaceServerFrame> m_frame{};
    Net::UnreliableSocket::Connection m_connection;
    Net::UnreliableSocket m_socket;

    static RaceClient *s_instance;
};

} // namespace SP
