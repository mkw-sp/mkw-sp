#pragma once

extern "C" {
#include <libhydrogen/hydrogen.h>
#include <revolution.h>
}

#include <optional>

namespace SP::Net {

class UnreliableSocket {
public:
    UnreliableSocket(hydro_kx_session_keypair keypair,
            const char context[hydro_secretbox_CONTEXTBYTES]);
    UnreliableSocket(UnreliableSocket &&) = delete;
    ~UnreliableSocket();

    bool ready() const;
    std::optional<u16> UnreliableSocket::read(u8 *message, u16 maxSize);
    bool write(const u8 *message, u16 size);

private:
    bool makeNonBlocking();

    hydro_kx_session_keypair m_keypair;
    char m_context[hydro_secretbox_CONTEXTBYTES];
    s32 m_handle = -1;
};

} // namespace SP::Net
