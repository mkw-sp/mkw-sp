#pragma once

extern "C" {
#include <libhydrogen/hydrogen.h>
#include <revolution.h>
}

#include <optional>

namespace SP::Net {

class Socket {
public:
    Socket(const char *hostname, u16 port, const u8 serverPK[hydro_kx_PUBLICKEYBYTES],
        const char context[hydro_secretbox_CONTEXTBYTES]);
    ~Socket();
    bool ok() const;
    std::optional<u16> read(u8 *message, u16 maxSize);
    bool write(const u8 *message, u16 size);

private:
    s32 m_handle = -1;
    hydro_kx_session_keypair m_keypair;
    u64 m_messageID = 0;
    char m_context[hydro_secretbox_CONTEXTBYTES];
};

} // namespace SP::Net
