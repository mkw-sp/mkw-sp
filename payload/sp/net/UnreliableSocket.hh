#pragma once

extern "C" {
#include <libhydrogen/hydrogen.h>
}

#include <optional>

#include <Common.hh>

namespace SP::Net {

// NOTE (vabold): Please do not ask me to explain any of this
class UnreliableSocket {
public:
    struct Connection {
        u32 ip;
        u16 port;
        hydro_kx_session_keypair keypair;
    };

    class ConnectionGroup {
    public:
        virtual u32 count() = 0;
        virtual Connection &operator[](u32 index) = 0;
    };

    struct Read {
        u16 size;
        u32 index;
    };

    UnreliableSocket(const char context[hydro_secretbox_CONTEXTBYTES], std::optional<u16> port);
    UnreliableSocket(const UnreliableSocket &) = delete;
    UnreliableSocket(UnreliableSocket &&) = delete;
    ~UnreliableSocket();

    std::optional<Read> read(u8 *message, u16 maxSize, ConnectionGroup &connectionGroup);
    bool write(const u8 *message, u16 size, const Connection &connection);

private:
    bool makeNonBlocking();

    char m_context[hydro_secretbox_CONTEXTBYTES];
    s32 m_handle = -1;
    std::optional<u16> m_port{};
};

} // namespace SP::Net
