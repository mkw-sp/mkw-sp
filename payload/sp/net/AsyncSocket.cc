#include "AsyncSocket.hh"

#include <common/Bytes.hh>

#include <cstring>

namespace SP::Net {

// XX variant, client-side
AsyncSocket::AsyncSocket(u32 ip, u16 port, const char context[hydro_secretbox_CONTEXTBYTES]) {
    m_handle = SOSocket(SO_PF_INET, SO_SOCK_STREAM, 0);
    if (m_handle < 0) {
        SP_LOG("Failed to create socket, returned %d", m_handle);
        return;
    }

    if (!makeNonBlocking()) {
        SOClose(m_handle);
        m_handle = -1;
        return;
    }

    SOSockAddrIn address{};
    address.len = sizeof(address);
    address.family = SO_PF_INET;
    address.port = port;
    address.addr.addr = ip;
    s32 result = SOConnect(m_handle, &address);
    if (result != SO_EINPROGRESS) {
        SP_LOG("Failed to initiate connection, returned %d", result);
        SOClose(m_handle);
        m_handle = -1;
        return;
    }

    m_connectTask = ConnectTask{address};

    m_initTask = InitTask{};
    m_initTask->isServer = false;
    hydro_kx_keygen(&m_initTask->keypair);
    if (hydro_kx_xx_1(&m_initTask->state, m_initTask->xx1, nullptr) != 0) {
        SP_LOG("Failed to run key exchange step XX1");
        SOClose(m_handle);
        m_handle = -1;
        return;
    }

    assert(strlen(context) == sizeof(m_context));
    memcpy(m_context, context, sizeof(m_context));
}

// XX variant, server-side
AsyncSocket::AsyncSocket(s32 handle, const hydro_kx_keypair &serverKeypair,
        const char context[hydro_secretbox_CONTEXTBYTES])
    : m_handle(handle) {
    if (!makeNonBlocking()) {
        SOClose(m_handle);
        m_handle = -1;
        return;
    }

    m_initTask = InitTask{};
    m_initTask->isServer = true;
    memcpy(&m_initTask->keypair, &serverKeypair, sizeof(m_initTask->keypair));

    assert(strlen(context) == sizeof(m_context));
    memcpy(m_context, context, sizeof(m_context));
}

AsyncSocket::~AsyncSocket() {
    if (m_initTask) {
        hydro_memzero(&*m_initTask, sizeof(*m_initTask));
    }
    hydro_memzero(&m_keypair, sizeof(m_keypair));
    if (m_handle >= 0) {
        SOClose(m_handle);
    }
}

hydro_kx_session_keypair AsyncSocket::keypair() const {
    return m_keypair;
}

bool AsyncSocket::ready() const {
    if (m_handle < 0) {
        return false;
    }

    if (m_connectTask || m_initTask) {
        return false;
    }

    return true;
}

bool AsyncSocket::poll() {
    if (m_handle < 0) {
        return false;
    }

    if (m_connectTask) {
        s32 result = SOConnect(m_handle, &m_connectTask->address);
        if (result == SO_EINPROGRESS || result == SO_EALREADY) {
            return true;
        } else if (result != SO_EISCONN && result != 0) {
            SP_LOG("Failed to connect, returned %d", result);
            return false;
        }
        m_connectTask.reset();
    }

    if (m_initTask && m_initTask->isServer) {
        if (m_initTask->xx1Offset < hydro_kx_XX_PACKET1BYTES) {
            if (!recv(m_initTask->xx1, hydro_kx_XX_PACKET1BYTES, m_initTask->xx1Offset)) {
                return false;
            }

            if (m_initTask->xx1Offset == hydro_kx_XX_PACKET1BYTES) {
                if (hydro_kx_xx_2(&m_initTask->state, m_initTask->xx2, m_initTask->xx1, nullptr,
                            &m_initTask->keypair) != 0) {
                    SP_LOG("Failed to run key exchange step XX2");
                    return false;
                }
            }
        }

        if (m_initTask->xx1Offset == hydro_kx_XX_PACKET1BYTES &&
                m_initTask->xx2Offset < hydro_kx_XX_PACKET2BYTES) {
            if (!send(m_initTask->xx2, hydro_kx_XX_PACKET2BYTES, m_initTask->xx2Offset)) {
                return false;
            }
        }

        if (m_initTask->xx2Offset == hydro_kx_XX_PACKET2BYTES) {
            if (!recv(m_initTask->xx3, hydro_kx_XX_PACKET3BYTES, m_initTask->xx3Offset)) {
                return false;
            }

            if (m_initTask->xx3Offset == hydro_kx_XX_PACKET3BYTES) {
                if (hydro_kx_xx_4(&m_initTask->state, &m_keypair, m_peerPK, m_initTask->xx3,
                            nullptr) != 0) {
                    SP_LOG("Failed to run key exchange step XX4");
                    return false;
                }

                SP_LOG("Successfully completed key exchange");
                m_initTask.reset();
            }
        }
    }

    if (m_initTask && !m_initTask->isServer) {
        if (m_initTask->xx1Offset < hydro_kx_XX_PACKET1BYTES) {
            if (!send(m_initTask->xx1, hydro_kx_XX_PACKET1BYTES, m_initTask->xx1Offset)) {
                return false;
            }
        }

        if (m_initTask->xx1Offset == hydro_kx_XX_PACKET1BYTES &&
                m_initTask->xx2Offset < hydro_kx_XX_PACKET2BYTES) {
            if (!recv(m_initTask->xx2, hydro_kx_XX_PACKET2BYTES, m_initTask->xx2Offset)) {
                return false;
            }

            if (m_initTask->xx2Offset == hydro_kx_XX_PACKET2BYTES) {
                if (hydro_kx_xx_3(&m_initTask->state, &m_keypair, m_initTask->xx3, m_peerPK,
                            m_initTask->xx2, nullptr, &m_initTask->keypair) != 0) {
                    SP_LOG("Failed to run key exchange step XX3");
                    return false;
                }
            }
        }

        if (m_initTask->xx2Offset == hydro_kx_XX_PACKET2BYTES) {
            if (!send(m_initTask->xx3, hydro_kx_XX_PACKET3BYTES, m_initTask->xx3Offset)) {
                return false;
            }

            if (m_initTask->xx3Offset == hydro_kx_XX_PACKET3BYTES) {
                SP_LOG("Successfully completed key exchange");
                m_initTask.reset();
            }
        }
    }

    if (m_initTask) {
        return true;
    }

    if (auto *writeTask = m_writeQueue.front()) {
        if (!send(writeTask->buffer, writeTask->size, writeTask->offset)) {
            return false;
        }

        if (writeTask->offset == writeTask->size) {
            m_writeQueue.pop_front();
        }
    }

    {
        auto *readTask = m_readQueue.back();
        if (!readTask || readTask->offset == sizeof(u16) + readTask->size) {
            m_readQueue.push_back({});
        }
    }

    if (auto *readTask = m_readQueue.back()) {
        if (readTask->offset < sizeof(u16)) {
            if (!recv(readTask->buffer, sizeof(u16), readTask->offset)) {
                return false;
            }

            if (readTask->offset == sizeof(u16)) {
                readTask->size = Bytes::Read<u16>(readTask->buffer, 0);
                if (readTask->size > sizeof(readTask->buffer) - sizeof(u16)) {
                    SP_LOG("Message %llu is larger than the allotted buffer size (0x%04X > 0x%04X)",
                            m_readMessageID + 1, readTask->size,
                            sizeof(readTask->buffer) - sizeof(u16));
                    return false;
                }
            }
        }

        if (readTask->offset >= sizeof(u16) && readTask->offset < readTask->size - sizeof(u16)) {
            if (!recv(readTask->buffer, sizeof(u16) + readTask->size, readTask->offset)) {
                return false;
            }
        }
    }

    return true;
}

std::expected<std::optional<u16>, const wchar_t *> AsyncSocket::read(u8 *message, u16 maxSize) {
    assert(m_handle >= 0);

    auto *readTask = m_readQueue.front();
    if (!readTask) {
        return std::unexpected(L"Socket is not ready!");
    }

    if (readTask->offset < sizeof(u16) + readTask->size) {
        return std::nullopt;
    }

    if (hydro_secretbox_HEADERBYTES + maxSize < readTask->size) {
        return std::unexpected(L"Failed to decrypt message");
    }
    if (hydro_secretbox_decrypt(message, readTask->buffer + sizeof(u16), readTask->size,
                m_readMessageID++, m_context, m_keypair.rx) != 0) {
        return std::unexpected(L"Failed to decrypt message");
    }
    u16 size = readTask->size - hydro_secretbox_HEADERBYTES;
    m_readQueue.pop_front();

    if (size == 0) {
        return std::nullopt;
    } else {
        return size;
    }
}

std::expected<void, const wchar_t *> AsyncSocket::write(const u8 *message, u16 size) {
    assert(m_handle >= 0);
    if (!ready()) {
        return std::unexpected(L"Cannot write messages until socket is ready!");
    }

    WriteTask writeTask{};
    writeTask.size = sizeof(u16) + hydro_secretbox_HEADERBYTES + size;
    assert(writeTask.size - sizeof(u16) <= UINT16_MAX);
    Bytes::Write<u16>(writeTask.buffer, 0, writeTask.size - sizeof(u16));
    if (hydro_secretbox_encrypt(writeTask.buffer + sizeof(u16), message, size, m_writeMessageID++,
                m_context, m_keypair.tx) != 0) {
        return std::unexpected(L"Failed to encrypt message");
    }
    if (!m_writeQueue.push_back(std::move(writeTask))) {
        return std::unexpected(L"Write queue is too full!");
    }

    return {};
}

bool AsyncSocket::makeNonBlocking() {
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

bool AsyncSocket::recv(u8 *buffer, u16 size, u16 &offset) {
    s32 result = SORecv(m_handle, buffer + offset, size - offset, 0);
    if (result > 0) {
        offset += result;
    } else if (result != SO_EAGAIN) {
        SP_LOG("Failed to receive packet, returned %d", result);
        return false;
    }
    return true;
}

bool AsyncSocket::send(const u8 *buffer, u16 size, u16 &offset) {
    s32 result = SOSend(m_handle, buffer + offset, size - offset, 0);
    if (result >= 0) {
        offset += result;
    } else if (result != SO_EAGAIN) {
        SP_LOG("Failed to send packet, returned %d", result);
        return false;
    }
    return true;
}

} // namespace SP::Net
