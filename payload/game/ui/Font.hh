#pragma once

extern "C" {
#include <Common.h>
}

namespace UI {

struct MessageInfo {
    s32 intVals[9];
    u32 messageIds[9];
    u8 _0x48[0xc4 - 0x48];
};
static_assert(sizeof(MessageInfo) == 0xc4);

} // namespace UI
