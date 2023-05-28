#pragma once

#include "game/kart/KartObjectProxy.hh"

namespace Kart {

class KartBullet : public KartObjectProxy {
public:
    void activateBullet(u32 r4);
    void cancelBullet();

private:
    u8 _0c[0x64 - 0x0c];
};

static_assert(sizeof(KartBullet) == 0x64);

} // namespace Kart
