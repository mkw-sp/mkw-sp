#pragma once

#include "KartPart.hh"

namespace Kart {

class KartSaveState;

struct KartSusPhysics {
    u8 _48[0x48 - 0x00];
};

class KartSus : public KartPart {
    friend class KartSaveState;

private:
    KartSusPhysics *m_physics;
};

static_assert(sizeof(KartSusPhysics) == 0x48);
static_assert(sizeof(KartSus) == 0x94);

} // namespace Kart
