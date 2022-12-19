#pragma once

#include "game/kart/KartObjectProxy.hh"

namespace Enemy {

class KartEnemy : public Kart::KartObjectProxy {
public:
    bool REPLACED(isCpu)();
    REPLACE bool isCpu();

private:
    u8 _0c[0x24 - 0x0c];
};
static_assert(sizeof(KartEnemy) == 0x24);

} // namespace Enemy
