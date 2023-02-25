#include "KartObjectManager.hh"

namespace Kart {

class WheelPhysics: public KartObjectProxy {
private:
    u8 _0c[0x84 - 0x0c];
};

struct KartTire {
    u8 _00[0x98 - 0x00];
    WheelPhysics *m_wheelPhysics;
    u8 _9c[0xd0 - 0x9c];
};

static_assert(sizeof(WheelPhysics) == 0x84);
static_assert(sizeof(KartTire) == 0xd0);

} // namespace Kart

