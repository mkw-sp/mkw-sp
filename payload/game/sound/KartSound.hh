#pragma once

#include "game/kart/KartObjectProxy.hh"

namespace Sound {

class KartSound {
public:
    void REPLACED(calcLap)();
    REPLACE void calcLap();

private:
    u8 _00[0xda - 0x00];
    bool m_hasFinished;
    u8 m_maxLap;
    Kart::KartObjectProxy *m_proxy;

public:
    bool m_isLocal;
    bool m_isGhost;

private:
    u8 _de[0xfc - 0xe2];
};
static_assert(sizeof(KartSound) == 0xfc);

} // namespace Sound
