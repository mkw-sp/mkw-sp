#pragma once

#include "game/kart/KartObject.hh"

namespace Kart {

class KartObjectManager {
public:
    KartObject *object(u32 playerId);

    static KartObjectManager *Instance();

private:
    u8 _00[0x20 - 0x00];
    KartObject **m_objects;
    u8 m_count;
    u8 _25[0x38 - 0x25];

    static KartObjectManager *s_instance;
};
static_assert(sizeof(KartObjectManager) == 0x38);

} // namespace Kart
