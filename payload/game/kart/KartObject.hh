#pragma once

#include "game/kart/KartObjectProxy.hh"

namespace Kart {

class KartObject : public KartObjectProxy {
private:
    u8 _0xc[0x80 - 0x0c];
};
static_assert(sizeof(KartObject) == 0x80);

} // namespace Kart
