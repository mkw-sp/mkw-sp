#pragma once

#include "KartObjectProxy.hh"

namespace Kart {

class KartPart: public KartObjectProxy {
public:
    KartPart();
    virtual ~KartPart();
private:
    u8 _10[0x90 - 0x10];
};

static_assert(sizeof(KartPart) == 0x90);

} // namespace Kart
