#pragma once

#include "game/system/Mii.hh"

namespace UI {

struct MessageInfo {
    s32 intVals[9];
    u32 messageIds[9];
    const System::Mii *miis[9];
    u8 _6c[0x9c - 0x6c];
    const wchar_t *strings[9];
    u8 _c0[0xc4 - 0xc0];
};
static_assert(sizeof(MessageInfo) == 0xc4);

} // namespace UI
