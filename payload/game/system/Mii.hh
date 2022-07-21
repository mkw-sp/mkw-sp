#pragma once

#include <Common.hh>

namespace System {

struct MiiId {
    u8 avatar[4];
    u8 client[4];
};
static_assert(sizeof(MiiId) == 0x8);

struct RawMii {
    u8 _00[0x02 - 0x00];
    wchar_t name[10];
    u8 _16[0x4a - 0x16];
    u16 crc16;
};
static_assert(sizeof(RawMii) == 0x4c);

struct Mii {
    u8 _00[0x68 - 0x00];
    wchar_t name[10];
    u8 _7c[0x94 - 0x7c];
    MiiId id;
    u8 _9c[0xb8 - 0x9c];
};
static_assert(sizeof(Mii) == 0xb8);

} // namespace System
