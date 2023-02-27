#pragma once

#include "KartObjectProxy.hh"

namespace Kart {

class KartBody: public KartPart {
    u8 _90[0x234 - 0x90];
};

static_assert(sizeof(KartBody) == 0x234);

} // namespace Kart
