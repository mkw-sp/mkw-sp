#pragma once

#include "KartObjectProxy.hh"

namespace Kart {

class KartBody: public KartObjectProxy {
    u8 _0c[0x234 - 0x0c];
};

static_assert(sizeof(KartBody) == 0x234);

} // namespace Kart
