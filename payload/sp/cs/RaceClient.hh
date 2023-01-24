#pragma once

#include "sp/CircularBuffer.hh"
#include "sp/cs/RaceManager.hh"
#include "sp/cs/RoomClient.hh"

namespace SP {

class RaceClient final : public RaceManager {
public:
    void destroyInstance() override;

    u32 frameCount() const;
    const std::optional<RaceServerFrame> &frame() const;
    /*s32 drift() const;
    void adjustDrift();*/

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

    RaceClient(RoomClient &roomClient);
    ~RaceClient();

    bool isFrameValid(const RaceServerFrame &frame);

    static bool IsVec3Valid(const PlayerFrame_Vec3 &v);
    static bool IsQuatValid(const PlayerFrame_Quat &q);
    static bool IsF32Valid(f32 s);

    RoomClient &m_roomClient;
    Net::UnreliableSocket m_socket;
    Net::UnreliableSocket::Connection m_connection;
    u32 m_frameCount = 0;
    std::optional<RaceServerFrame> m_frame{};
    /*CircularBuffer<s32, 60> m_drifts;
    s32 m_drift = 0;*/

    static RaceClient *s_instance;
};

} // namespace SP
