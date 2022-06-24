#include "Socket.hh"

extern "C" {
#include "sp/Host.h"
}
#include "sp/net/Net.hh"

#include <common/Bytes.hh>

#include <cstdio>
#include <cstring>

namespace SP::Net {

Socket::Socket(const char *hostname, u16 port, const u8 serverPK[hydro_kx_PUBLICKEYBYTES],
        const char context[hydro_secretbox_CONTEXTBYTES]) {
    char service[0x10];
    snprintf(service, sizeof(service), "%u", port);
    SOAddrInfo hints{};
    hints.sockType = SO_SOCK_STREAM;
    SOAddrInfo *list;
    s32 result = SOGetAddrInfo(hostname, service, &hints, &list);
    if (result != 0) {
        SP_LOG("Failed to get address information, returned %d", result);
        return;
    }
    for (SOAddrInfo *ptr = list; ptr; ptr = ptr->next) {
        auto *address = reinterpret_cast<SOSockAddrIn *>(ptr->addr);
        if (address->len < sizeof(SOSockAddrIn)) {
            continue;
        }
        address->len = sizeof(SOSockAddrIn);
        m_handle = SOSocket(ptr->family, ptr->sockType, 0);
        if (m_handle < 0) {
            SP_LOG("Failed to create socket, returned %d", m_handle);
            continue;
        }
        if (HostPlatform_IsConsole(Host_GetPlatform())) {
            u32 size = 0x8000;
            result = SOSetSockOpt(m_handle, SO_SOL_SOCKET, SO_SO_RCVBUF, &size, sizeof(size));
            if (result != 0) {
                SP_LOG("Failed to change the buffer size, returned %d", result);
                SOClose(m_handle);
                m_handle = -1;
                continue;
            }
        }
        result = SOConnect(m_handle, address);
        if (result != 0) {
            SP_LOG("Failed to connect, returned %d", result);
            SOClose(m_handle);
            m_handle = -1;
            continue;
        }
        break;
    }
    SOFreeAddrInfo(list);
    if (m_handle < 0) {
        return;
    }

    auto tmp = Alloc<u8>(hydro_kx_N_PACKET1BYTES);
    if (hydro_kx_n_1(&m_keypair, tmp.get(), nullptr, serverPK) != 0) {
        SP_LOG("Failed to generate key exchange packet");
        SOClose(m_handle);
        m_handle = -1;
        return;
    }
    result = SOSend(m_handle, tmp.get(), GetSize(tmp), 0);
    if (result != static_cast<s32>(GetSize(tmp))) {
        SP_LOG("Failed to send key exchange packet, returned %d", result);
        SOClose(m_handle);
        m_handle = -1;
        return;
    }

    assert(strlen(context) == sizeof(m_context));
    memcpy(m_context, context, sizeof(m_context));
}

Socket::~Socket() {
    hydro_memzero(&m_keypair, sizeof(m_keypair));
    if (m_handle >= 0) {
        SOClose(m_handle);
    }
}

bool Socket::ok() const {
    return m_handle >= 0;
}

bool Socket::read(u8 *message, u16 size) {
    auto tmp = Alloc<u8>(hydro_secretbox_HEADERBYTES + size);
    for (u16 offset = 0; offset < GetSize(tmp);) {
        s32 result = SORecv(m_handle, tmp.get() + offset, GetSize(tmp) - offset, 0);
        if (result <= 0) {
            SP_LOG("Failed to receive message, returned %d", result);
            return false;
        }
        offset += result;
    }
    const u8 *key = m_keypair.rx;
    if (hydro_secretbox_decrypt(message, tmp.get(), GetSize(tmp), m_messageID++, m_context, key)
            != 0) {
        SP_LOG("Failed to decrypt message");
        return false;
    }
    return true;
}

bool Socket::write(const u8 *message, u16 size) {
    const u8 *key = m_keypair.tx;
    auto tmp = Alloc<u8>(sizeof(u16) + hydro_secretbox_HEADERBYTES + size);
    assert(GetSize(tmp) - 2 <= UINT16_MAX);
    Bytes::Write<u16>(tmp.get(), 0, GetSize(tmp) - 2);
    if (hydro_secretbox_encrypt(tmp.get() + sizeof(u16), message, size, m_messageID++, m_context,
            key) != 0) {
        SP_LOG("Failed to encrypt message");
        return false;
    }
    s32 result = SOSend(m_handle, tmp.get(), GetSize(tmp), 0);
    if (result != static_cast<s32>(GetSize(tmp))) {
        SP_LOG("Failed to send message, returned %d", result);
        return false;
    }
    return true;
}

} // namespace SP::Net
