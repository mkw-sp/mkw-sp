#pragma once

#include <Common.hh>

namespace UI {

class MessageGroup {
public:
    MessageGroup();
    void dt(s32 type);

private:
    u8 _00[0x14 - 0x00];
};

} // namespace UI
