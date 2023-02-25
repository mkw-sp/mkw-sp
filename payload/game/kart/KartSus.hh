#include "KartObjectProxy.hh"

namespace Kart {

class KartSaveState;

struct KartSusBase {
    u8 _80[0x80 - 0x00];
};

struct KartSusPhysics {
    u8 _48[0x48 - 0x00];
};

class KartSus: public KartObjectProxy {
    friend class KartSaveState;

private:
    u8 _0x0c[0x10 - 0x0c];
    KartSusBase m_base;
    KartSusPhysics *m_physics;
};

static_assert(sizeof(KartSusBase) == 0x80);
static_assert(sizeof(KartSusPhysics) == 0x48);
static_assert(sizeof(KartSus) == 0x94);

} // namespace Kart
