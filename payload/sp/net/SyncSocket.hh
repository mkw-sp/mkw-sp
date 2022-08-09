#pragma once

extern "C" {
#include <libhydrogen/hydrogen.h>
}

#include <optional>

#include <Common.hh>

namespace SP::Net {

class SyncSocket {
public:
    // N variant, client-side
    SyncSocket(const char *hostname, u16 port, const u8 serverPK[hydro_kx_PUBLICKEYBYTES],
            const char context[hydro_secretbox_CONTEXTBYTES]);
    SyncSocket(const SyncSocket &) = delete;
    SyncSocket(SyncSocket &&);
    SyncSocket &operator=(SyncSocket &&);
    ~SyncSocket();

    bool ok() const;
    std::optional<u16> read(u8 *message, u16 maxSize);
    bool write(const u8 *message, u16 size);

private:
    s32 m_handle = -1;
    hydro_kx_session_keypair m_keypair;
    char m_context[hydro_secretbox_CONTEXTBYTES];
    u64 m_messageID = 0;
};

} // namespace SP::Net
