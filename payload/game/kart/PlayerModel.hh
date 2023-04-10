#pragma once

#include "KartObjectProxy.hh"

namespace Kart {

class PlayerModel : public KartObjectProxy {
private:
    u8 _0c[0x6e4 - 0x00c];
};

static_assert(sizeof(PlayerModel) == 0x6e4);

} // namespace Kart
