#pragma once

#include <Common.hh>

#include "game/item/ItemInventory.hh"
#include "game/kart/KartObjectProxy.hh"

namespace Item {

// Tentative name
class KartItem : public Kart::KartObjectProxy {
public:
    void REPLACED(setItem)(u32 r4, u32 r5, u32 r6);
    REPLACE void setItem(u32 r4, u32 r5, u32 r6);

    ItemId nextItem();

    void REPLACED(calc)();
    REPLACE void calc();

private:
    u8 _000[0x018 - 0x00c];
    u8 m_playerId;
    u8 _019[0x088 - 0x019];
    ItemInventory m_inventory;
    u8 _0b4[0x248 - 0x0b4];
};
static_assert(sizeof(KartItem) == 0x248);

} // namespace Item
