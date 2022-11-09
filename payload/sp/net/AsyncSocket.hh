#pragma once

#include "sp/CircularBuffer.hh"

extern "C" {
#include <libhydrogen/hydrogen.h>
#include <revolution.h>
}

namespace SP::Net {

class AsyncSocket {
public:
    // XX variant, client-side
    AsyncSocket(u32 ip, u16 port, const char context[hydro_secretbox_CONTEXTBYTES]);
    // XX variant, server-side
    AsyncSocket(s32 handle, const hydro_kx_keypair &serverKeypair,
            const char context[hydro_secretbox_CONTEXTBYTES]);
    AsyncSocket(const AsyncSocket &) = delete;
    AsyncSocket(AsyncSocket &&) = delete;
    ~AsyncSocket();

    bool ready() const;
    bool poll();
    std::optional<u16> read(u8 *message, u16 maxSize);
    bool write(const u8 *message, u16 size);

    hydro_kx_session_keypair &getKeypair() {
        return m_keypair;
    }

    u32 getIp() const {
        return m_ip;
    }

    u16 getPort() const {
        return m_port;
    }

private:
    struct ConnectTask {
        SOSockAddrIn address;
    };

    struct InitTask {
        bool isServer;
        hydro_kx_keypair keypair;
        hydro_kx_state state;
        u8 xx1[hydro_kx_XX_PACKET1BYTES];
        u16 xx1Offset = 0;
        u8 xx2[hydro_kx_XX_PACKET2BYTES];
        u16 xx2Offset = 0;
        u8 xx3[hydro_kx_XX_PACKET3BYTES];
        u16 xx3Offset = 0;
    };

    struct ReadTask {
        u8 buffer[1024];
        u16 size = 0;
        u16 offset = 0;
    };

    struct WriteTask {
        u8 buffer[1024];
        u16 size = 0;
        u16 offset = 0;
    };

    bool makeNonBlocking();
    bool recv(u8 *buffer, u16 size, u16 &offset);
    bool send(const u8 *buffer, u16 size, u16 &offset);

    s32 m_handle = -1;
    u8 m_peerPK[hydro_kx_PUBLICKEYBYTES];
    hydro_kx_session_keypair m_keypair;
    char m_context[hydro_secretbox_CONTEXTBYTES];
    std::optional<ConnectTask> m_connectTask{};
    std::optional<InitTask> m_initTask{};
    u64 m_readMessageID = 0;
    u64 m_writeMessageID = 0;
    CircularBuffer<ReadTask, 32> m_readQueue;
    CircularBuffer<WriteTask, 32> m_writeQueue;

    // NOTE: For now
    u32 m_ip;
    u16 m_port;
};

} // namespace SP::Net
