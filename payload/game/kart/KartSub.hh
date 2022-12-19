#pragma once

#include "game/kart/KartSettings.hh"
#include "game/kart/KartState.hh"

namespace Kart {

// TODO this cannot be the actual name, even if it fits, right?
class KartSub {
public:
    void REPLACED(createComponents)(KartSettings *settings);
    REPLACE void createComponents(KartSettings *settings);

private:
    u8 _00[0x1c - 0x00];
    KartState *m_state;
    u8 _20[0x9c - 0x20];
};
static_assert(sizeof(KartSub) == 0x9c);

} // namespace Kart
