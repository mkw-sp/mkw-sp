#include "AsyncListener.hh"

extern "C" {
#include <libhydrogen/hydrogen.h>
#include <revolution.h>
}

namespace SP::Net {

AsyncListener::AsyncListener(u16 port) {
    m_handle = SOSocket(SO_PF_INET, SO_SOCK_STREAM, 0);
    if (m_handle < 0) {
        SP_LOG("Failed to create socket, returned %d", m_handle);
        return;
    }

    s32 result = SOFcntl(m_handle, SO_F_GETFL, 0);
    if (result < 0) {
        SP_LOG("Failed to get status flags, returned %d", result);
        SOClose(m_handle);
        m_handle = -1;
        return;
    }

    result = SOFcntl(m_handle, SO_F_SETFL, result | SO_O_NONBLOCK);
    if (result != 0) {
        SP_LOG("Failed to set status flags, returned %d", result);
        SOClose(m_handle);
        m_handle = -1;
        return;
    }

    m_bindTask = { port, 0, 0 };
}

AsyncListener::~AsyncListener() {
    if (m_handle >= 0) {
        SOClose(m_handle);
    }
}

bool AsyncListener::ready() const {
    if (m_handle < 0) {
        return false;
    }

    if (m_bindTask) {
        return false;
    }

    return true;
}

bool AsyncListener::poll() {
    if (m_handle < 0) {
        return false;
    }

    if (m_bindTask) {
        SOSockAddrIn address{};
        address.len = sizeof(address);
        address.family = SO_PF_INET;
        if (m_bindTask->port == 0) {
            // Unlike on most other platforms, passing port 0 to let the backend assign a random
            // free port is not supported on the Wii.
            if (m_bindTask->attemptPort == 0) {
                m_bindTask->attemptPort = 49152 + hydro_random_uniform(16384);
            }
            address.port = m_bindTask->attemptPort;
            s32 result = SOBind(m_handle, &address);
            if (result == SO_EINPROGRESS || result == SO_EALREADY) {
                return true;
            } else if (result == SO_EADDRINUSE) {
                m_bindTask->attempt++;
                if (m_bindTask->attempt < 1024) {
                    return true;
                } else {
                    SP_LOG("Failed to find a free port");
                    return false;
                }
            } else if (result != 0) {
                SP_LOG("Failed to bind address to socket, returned %d", result);
                return false;
            }
        } else {
            address.port = m_bindTask->port;
            s32 result = SOBind(m_handle, &address);
            if (result == SO_EINPROGRESS || result == SO_EALREADY) {
                return true;
            } else if (result != 0) {
                SP_LOG("Failed to bind address to socket, returned %d", result);
                return false;
            }
        }
        m_bindTask.reset();
        s32 result = SOListen(m_handle, 8);
        if (result != 0) {
            SP_LOG("Failed to listen to address, returned %d", result);
            return false;
        }
    }

    return true;
}

std::optional<s32> AsyncListener::accept() {
    if (m_bindTask) {
        return {};
    }

    s32 result = SOAccept(m_handle, nullptr);
    if (result < 0) {
        return {};
    }
    return result;
}

} // namespace SP::Net
