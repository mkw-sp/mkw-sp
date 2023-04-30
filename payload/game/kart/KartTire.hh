#pragma once

#include "KartObjectManager.hh"

namespace Kart {

class WheelPhysics : public KartObjectProxy {
private:
    u8 _0c[0x84 - 0x0c];
};

class KartTire : public KartPart {
    friend class KartSaveState;

private:
    u8 _90[0x98 - 0x90];
    WheelPhysics *m_wheelPhysics;
    u8 _9c[0xd0 - 0x9c];
};

static_assert(sizeof(WheelPhysics) == 0x84);
static_assert(sizeof(KartTire) == 0xd0);

} // namespace Kart
