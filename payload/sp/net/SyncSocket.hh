#pragma once

#include "sp/net/Socket.hh"

extern "C" {
#include <libhydrogen/hydrogen.h>
}

namespace SP::Net {

class SyncSocket : public Socket {
public:
    // N variant, client-side
    SyncSocket(const char *hostname, u16 port, const u8 serverPK[hydro_kx_PUBLICKEYBYTES],
            const char context[hydro_secretbox_CONTEXTBYTES]);
    SyncSocket(const SyncSocket &) = delete;
    SyncSocket(SyncSocket &&);
    SyncSocket &operator=(SyncSocket &&);
    ~SyncSocket();

    bool ok() const;
    std::expected<std::optional<u16>, const wchar_t *> read(u8 *message, u16 maxSize) override;
    std::expected<void, const wchar_t *> write(const u8 *message, u16 size) override;

private:
    s32 m_handle = -1;
    hydro_kx_session_keypair m_keypair;
    char m_context[hydro_secretbox_CONTEXTBYTES];
    u64 m_messageID = 0;
};

} // namespace SP::Net
