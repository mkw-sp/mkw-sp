#pragma once

extern "C" {
#include <revolution.h>
}

#include <optional>

namespace SP::Net {

class AsyncListener {
public:
    struct Connection {
        s32 handle;
        SOSockAddrIn address;
    };

    AsyncListener(u16 port);
    AsyncListener(const AsyncListener &) = delete;
    AsyncListener(AsyncListener &&) = delete;
    ~AsyncListener();

    bool ready() const;
    bool poll();
    std::optional<Connection> accept();

private:
    struct BindTask {
        u16 port;
        u16 attempt;
        u16 attemptPort;
    };

    s32 m_handle = -1;
    std::optional<BindTask> m_bindTask;
};

} // namespace SP::Net
