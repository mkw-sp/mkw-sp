#pragma once

#include <optional>

#include <Common.hh>

namespace SP::Net {

class AsyncListener {
public:
    AsyncListener(u16 port);
    AsyncListener(const AsyncListener &) = delete;
    AsyncListener(AsyncListener &&) = delete;
    ~AsyncListener();

    bool ready() const;
    bool poll();
    std::optional<s32> accept();

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
