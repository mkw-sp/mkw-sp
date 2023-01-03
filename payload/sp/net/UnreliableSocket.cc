#include "UnreliableSocket.hh"

#include <common/Bytes.hh>
#include <sp/cs/RoomManager.hh>

#include <cstring>

namespace SP::Net {

UnreliableSocket::UnreliableSocket(const char context[hydro_secretbox_CONTEXTBYTES],
        std::optional<u16> port) : m_port(port) {
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
    if (m_handle >= 0) {
        SOClose(m_handle);
    }
}

std::optional<UnreliableSocket::Read> UnreliableSocket::read(u8 *message, u16 maxSize,
        ConnectionGroup &connectionGroup) {
    assert(m_handle >= 0);

    if (m_port) {
        SOSockAddrIn address{};
        address.len = sizeof(address);
        address.family = SO_PF_INET;
        address.port = *m_port;
        s32 result = SOBind(m_handle, &address);
        if (result == SO_EINPROGRESS || result == SO_EALREADY) {
            return {};
        } else if (result != 0) {
            SP_LOG("Failed to bind address to socket, returned %d", result);
            return {};
        }

        m_port.reset();
    }

    for (u8 iterations = 0; iterations < 5; iterations++) {
        SOSockAddrIn address{};
        address.len = sizeof(address);
        address.family = SO_PF_INET;

        u8 buffer[1024];
        s32 result = SORecvFrom(m_handle, buffer, sizeof(buffer), 0, &address);

        if (result == SO_EAGAIN) {
            return {};
        } else if (result < 0) {
            SP_LOG("Failed to receive packet, returned %d", result);
            return {};
        }

        if (hydro_secretbox_HEADERBYTES + maxSize < result) {
            SP_LOG("Failed to decrypt message");
            continue;
        }

        for (u8 i = 0; i < connectionGroup.count(); i++) {
            if (hydro_secretbox_decrypt(message, buffer, result, 0, m_context,
                        connectionGroup[i].keypair.rx) == 0) {
                // TODO: this sucks
                connectionGroup[i].ip = address.addr.addr;
                connectionGroup[i].port = address.port;
                return Read{static_cast<u16>(result - hydro_secretbox_HEADERBYTES), i};
            }
        }
    }

    return {};
}

bool UnreliableSocket::write(const u8 *message, u16 size, const Connection &connection) {
    assert(m_handle >= 0);

    u8 buffer[1024];
    assert(size + hydro_secretbox_HEADERBYTES > size);
    assert(static_cast<u32>(size + hydro_secretbox_HEADERBYTES) <= sizeof(buffer));
    if (hydro_secretbox_encrypt(buffer, message, size, 0, m_context, connection.keypair.tx) != 0) {
        SP_LOG("Failed to encrypt message");
        return false;
    }
    size += hydro_secretbox_HEADERBYTES;

    SOSockAddrIn address{};
    address.len = sizeof(address);
    address.family = SO_PF_INET;
    address.port = connection.port;
    address.addr.addr = connection.ip;
    s32 result = SOSendTo(m_handle, buffer, size, 0, &address);
    if (result == SO_EAGAIN) {
        return false;
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
