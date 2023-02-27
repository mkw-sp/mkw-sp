#include "KartObjectProxy.hh"

namespace Kart {

class Kart5c: public KartObjectProxy {
private:
    u8 _0x0c[0x14 - 0x0c];
};

static_assert(sizeof(Kart5c) == 0x14);

} // namespace Kart
