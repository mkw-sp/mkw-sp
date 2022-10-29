#pragma once

#include "game/kart/KartObjectProxy.hh"

namespace Kart {

class KartBlink : public KartObjectProxy {
public:
    void REPLACED(calc)(bool stop);
    REPLACE void calc(bool stop);

private:
    u8 _0c[0x10 - 0x0c];
    s16 m_frame;
    bool m_visible;
    u8 _13[0x14 - 0x13];
};
static_assert(sizeof(KartBlink) == 0x14);

} // namespace Kart
