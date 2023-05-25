#pragma once

#include "game/kart/KartObjectProxy.hh"
extern "C" {
#include "game/sound/DriverSound.h"
}

namespace Race {

class Driver : public Kart::KartObjectProxy {
public:
    void REPLACED(onIndirectHit)(u32 receiver);
    REPLACE void onIndirectHit(u32 receiver);

    DriverSound *sound;

private:
    u8 _010[0x6e4 - 0x010];
};
static_assert(sizeof(Driver) == 0x6e4);

} // namespace Race
