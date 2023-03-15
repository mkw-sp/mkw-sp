#pragma once

#include "KartObjectProxy.hh"

namespace Kart {

class KartAction : public KartObjectProxy {
private:
    u8 _0c[0x100 - 0x0c];
};

static_assert(sizeof(KartAction) == 0x100);

} // namespace Kart
