#pragma once

#include "game/kart/KartObjectProxy.hh"

namespace Item {

// Tentative name
class KartItem : public Kart::KartObjectProxy {
public:
    void REPLACED(setItem)(u32 r4, u32 r5, u32 r6);
    REPLACE void setItem(u32 r4, u32 r5, u32 r6);

private:
    u8 _000[0x018 - 0x00c];
    u8 m_playerId;
    u8 _019[0x248 - 0x019];
};
static_assert(sizeof(KartItem) == 0x248);

} // namespace Item
