#pragma once

#include <Common.hh>

namespace UI {

class CountDownTimer {
public:
    bool isRunning() const;

private:
    f32 m_value;
    bool m_isRunning;
    u8 _05[0x0C - 0x05];
};
static_assert(sizeof(CountDownTimer) == 0x0C);

} // namespace UI
