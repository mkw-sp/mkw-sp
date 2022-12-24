#pragma once

#include "game/kart/KartObjectProxy.hh"
#include "game/kart/KartSub.hh"

namespace Kart {

class KartObject : public KartObjectProxy {
public:
    void REPLACED(init)();
    REPLACE void init();
    REPLACE void calcEarly();
    void REPLACED(calcLate)();
    REPLACE void calcLate();

private:
    u8 _0x0c[0x10 - 0x0c];
    KartSub *m_sub;
    KartSettings *m_settings;
    u8 _0x18[0x1c - 0x18];
    KartAccessor m_accessor;
};
static_assert(sizeof(KartObject) == 0x80);

} // namespace Kart
