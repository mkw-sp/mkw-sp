#include "UnreliableSocket.hh"

#include <common/Bytes.hh>
#include <sp/cs/RoomManager.hh>

#include <cstring>

namespace SP::Net {

// Client-side
UnreliableSocket::UnreliableSocket(ConnectionInfo connectionInfo,
        const char context[hydro_secretbox_CONTEXTBYTES]) {
    m_connections[0] = connectionInfo;

    m_handle = SOSocket(SO_PF_INET, SO_SOCK_DGRAM, 0);
    if (m_handle < 0) {
        SP_LOG("Failed to create socket, returned %d", m_handle);
        return;
    }

    if (!makeNonBlocking()) {
        SOClose(m_handle);
        m_handle = -1;
        return;
    }

    assert(strlen(context) == sizeof(m_context));
    memcpy(m_context, context, sizeof(m_context));
}

// Server-side
UnreliableSocket::UnreliableSocket(std::array<ConnectionInfo, 24> &connections,
        const char context[hydro_secretbox_CONTEXTBYTES], u32 connectionCount, u16 port)
    : m_connections(connections), m_connectionCount(connectionCount), m_port(port) {
    m_handle = SOSocket(SO_PF_INET, SO_SOCK_DGRAM, 0);
    if (m_handle < 0) {
        SP_LOG("Failed to create socket, returned %d", m_handle);
        return;
    }

    if (!makeNonBlocking()) {
        SOClose(m_handle);
        m_handle = -1;
        return;
    }

    assert(strlen(context) == sizeof(m_context));
    memcpy(m_context, context, sizeof(m_context));
}

UnreliableSocket::~UnreliableSocket() {
    hydro_memzero(&m_connections, sizeof(m_connections));
    if (m_handle >= 0) {
        SOClose(m_handle);
    }
}

std::optional<UnreliableSocket::ReadInfo> UnreliableSocket::read(u8 *message, u16 maxSize) {
    assert(m_handle >= 0);

    for (u8 iterations = 0; iterations < 5; iterations++) {
        SOSockAddrIn address{};
        address.len = sizeof(address);
        address.family = SO_PF_INET;

        u8 buffer[1024];
        s32 result = SORecvFrom(m_handle, buffer, sizeof(buffer), 0, &address);

        if (result == SO_EAGAIN) {
            return ReadInfo{0, 200};
        } else if (result < 0) {
            SP_LOG("Failed to receive packet, returned %d", result);
            return {};
        }

        if (hydro_secretbox_HEADERBYTES + maxSize < result) {
            SP_LOG("Failed to decrypt message");
            return {};
        }

        for (u8 i = 0; i < m_connectionCount; i++) {
            if (hydro_secretbox_decrypt(message, buffer, result, 0, m_context,
                        m_connections[i].keypair.rx) == 0) {
                // TODO: this sucks
                m_connections[i].ip = address.addr.addr;
                m_connections[i].port = address.port;
                return ReadInfo{static_cast<u16>(result - hydro_secretbox_HEADERBYTES), i};
            }
        }
    }

    return ReadInfo{0, 200};
}

bool UnreliableSocket::write(const u8 *message, u16 size, u8 idx) {
    assert(m_handle >= 0);

    if (m_port) {
        SOSockAddrIn address{};
        address.len = sizeof(address);
        address.family = SO_PF_INET;
        address.port = *m_port;
        s32 result = SOBind(m_handle, &address);
        if (result == SO_EINPROGRESS || result == SO_EALREADY) {
            return true;
        } else if (result != 0) {
            SP_LOG("Failed to bind address to socket, returned %d", result);
            return false;
        }

        m_port.reset();
    }

    u8 buffer[1024];
    assert(size + hydro_secretbox_HEADERBYTES > size);
    size += hydro_secretbox_HEADERBYTES;
    assert(size < sizeof(buffer));
    Bytes::Write<u16>(buffer, 0, size);
    if (hydro_secretbox_encrypt(buffer, message, size, 0, m_context,
                m_connections[idx].keypair.tx) != 0) {
        SP_LOG("Failed to encrypt message");
        return false;
    }

    SOSockAddrIn address{};
    address.len = sizeof(address);
    address.family = SO_PF_INET;
    address.port = m_connections[idx].port;
    address.addr.addr = m_connections[idx].ip;
    s32 result = SOSendTo(m_handle, buffer, size, 0, &address);
    if (result == SO_EAGAIN) {
        return true;
    } else if (result < 0) {
        SP_LOG("Failed to send packet, returned %d", result);
        return false;
    }
    return true;
}

bool UnreliableSocket::makeNonBlocking() {
    s32 result = SOFcntl(m_handle, SO_F_GETFL, 0);
    if (result < 0) {
        SP_LOG("Failed to get status flags, returned %d", result);
        return false;
    }

    result = SOFcntl(m_handle, SO_F_SETFL, result | SO_O_NONBLOCK);
    if (result != 0) {
        SP_LOG("Failed to set status flags, returned %d", result);
        return false;
    }

    return true;
}

} // namespace SP::Net
