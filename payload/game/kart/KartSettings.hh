#pragma once

#include "KartObjectProxy.hh"

namespace Kart {

struct KartSettings {
    u32 isBike;
    u32 vehicle;
    u32 character;
    u16 susCount;
    u16 tireCount;
    u8 playerId;
    u8 _11[0x3c - 0x11];
};
static_assert(sizeof(KartSettings) == 0x3c);

} // namespace Kart
