#pragma once

#include <Common.hh>

namespace Kart {

struct KartSettings {
    u8 _00[0x10 - 0x00];
    u8 playerId;
    u8 _11[0x3c - 0x11];
};
static_assert(sizeof(KartSettings) == 0x3c);

} // namespace Kart
