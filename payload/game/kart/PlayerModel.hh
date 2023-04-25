#include "KartObjectProxy.hh"

extern "C" {
#include "game/sound/DriverSound.h"
}

namespace Kart {

class PlayerModel : public KartObjectProxy {
public:
    DriverSound *sound;

private:
    u8 _0c[0x6e4 - 0x010];
};

static_assert(sizeof(PlayerModel) == 0x6e4);

} // namespace Kart
