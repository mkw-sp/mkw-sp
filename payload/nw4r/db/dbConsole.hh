#pragma once

#include <Common.hh>

namespace nw4r::db {

namespace detail {

struct ConsoleHead {
    u8 _00[0x14 - 0x00];
    s32 m_lineCount;
    s32 m_currentTopLine;
    s16 m_xPos;
    u8 _1e[0x22 - 0x1e];
    bool m_visible;
    u8 _23[0x2c - 0x23];
};
static_assert(sizeof(ConsoleHead) == 0x2c);

} // namespace detail

typedef detail::ConsoleHead *ConsoleHandle;

void Console_DrawDirect(ConsoleHandle console);

s32 Console_GetTotalLines(ConsoleHandle console);

} // namespace nw4r::db
