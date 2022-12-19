#pragma once

#include <Common.hh>

namespace Effect {

class KartEffect {
private:
    u8 _000[0x129 - 0x000];

public:
    bool m_isLocal;
    bool m_isLod;
    bool m_isGhost;

private:
    u8 _12c[0xae8 - 0x12c];
};
static_assert(sizeof(KartEffect) == 0xae8);

} // namespace Effect
