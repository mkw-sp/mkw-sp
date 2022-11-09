#include "UnreliableSocket.hh"

#include <common/Bytes.hh>
#include <sp/cs/RoomManager.hh>

namespace SP::Net {

UnreliableSocket::UnreliableSocket(hydro_kx_session_keypair keypair,
        const char context[hydro_secretbox_CONTEXTBYTES])
    : m_keypair(keypair) {
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

    assert(strlen(context) == sizeof(context));
    memcpy(m_context, context, sizeof(context));
}

UnreliableSocket::~UnreliableSocket() {
    hydro_memzero(&m_keypair, sizeof(m_keypair));
    if (m_handle >= 0) {
        SOClose(m_handle);
    }
}

bool UnreliableSocket::ready() const {
    return m_handle >= 0;
}

std::optional<u16> UnreliableSocket::read(u8 *message, u16 maxSize) {
    assert(m_handle >= 0);

    u8 buffer[1024];
    s32 result = SORecv(m_handle, &buffer, sizeof(buffer), 0);

    if (result == SO_EAGAIN) {
        return 0;
    } else if (result < 0) {
        SP_LOG("Failed to receive packet, returned %d", result);
        return {};
    }

    if (hydro_secretbox_HEADERBYTES + maxSize < result) {
        SP_LOG("Failed to decrypt message");
        return {};
    }

    if (hydro_secretbox_decrypt(message, buffer, result, 0, m_context, m_keypair.rx) != 0) {
        SP_LOG("Failed to decrypt message");
        return {};
    }

    return result - hydro_secretbox_HEADERBYTES;
}

bool UnreliableSocket::write(const u8 *message, u16 size) {
    assert(m_handle >= 0);

    u8 buffer[1024];
    assert(size + hydro_secretbox_HEADERBYTES > size);
    size += hydro_secretbox_HEADERBYTES;
    assert(size < sizeof(buffer));
    Bytes::Write<u16>(buffer, 0, size);
    if (hydro_secretbox_encrypt(buffer, message, size, 0,
            m_context, m_keypair.tx) != 0) {
        SP_LOG("Failed to encrypt message");
        return false;
    }

    s32 result = SOSend(m_handle, buffer, size, 0);
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
