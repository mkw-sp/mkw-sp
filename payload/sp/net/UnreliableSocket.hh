#pragma once

extern "C" {
#include <libhydrogen/hydrogen.h>
#include <revolution.h>
}

#include <array>
#include <optional>

#include <Common.hh>

namespace SP::Net {

// NOTE (vabold): Please do not ask me to explain any of this
class UnreliableSocket {
public:
    struct ReadInfo {
        u16 size;
        u8 playerIdx;
    };

    struct ConnectionInfo {
        u32 ip;
        u16 port;
        hydro_kx_session_keypair keypair;
    };

    UnreliableSocket(ConnectionInfo connectionInfo,
            const char context[hydro_secretbox_CONTEXTBYTES]);
    UnreliableSocket(std::array<ConnectionInfo, 24> &connections,
            const char context[hydro_secretbox_CONTEXTBYTES], u32 connectionCount, u16 port);
    UnreliableSocket(const UnreliableSocket &) = delete;
    UnreliableSocket(UnreliableSocket &&) = delete;
    ~UnreliableSocket();

    std::optional<ReadInfo> read(u8 *message, u16 maxSize);
    bool write(const u8 *message, u16 size, u8 idx);

private:
    bool makeNonBlocking();

    std::array<ConnectionInfo, 24> m_connections;
    char m_context[hydro_secretbox_CONTEXTBYTES];
    s32 m_handle = -1;
    u32 m_connectionCount = 0;
    std::optional<u16> m_port = 0;
};

} // namespace SP::Net
