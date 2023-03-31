#pragma once

#include <Common.hh>
#include "game/kart/KartObjectProxy.hh"

namespace Item {

enum Items {
        Green,
        Red,
        Nana,
        FIB,
        Shroom,
        TripShrooms,
        Bomb,
        Blue,
        Shock,
        Star,
        Golden,
        Mega,
        Blooper,
        Pow,
        TC,
        Bill,
        TripGreens,
        TripReds,
        TripNanas,
        Unused,
        NoItem,
};

struct PlayerInventory {
        u8 _00[0x04 - 0x00];
        u32 currentItemID;
        u32 currentItemCount;
        u8 _0c[0x2b - 0xc];
};
// Tentative name
class KartItem : public Kart::KartObjectProxy {
public:
    void REPLACED(setItem)(u32 r4, u32 r5, u32 r6);
    REPLACE void setItem(u32 r4, u32 r5, u32 r6);

    void REPLACED(update)();
    REPLACE void update();

private:
    u8 _000[0x018 - 0x00c];
    u8 m_playerId;
    u8 _019[0x088 - 0x019];
    PlayerInventory inventory;
    u8 _0b4[0x248 - 0x0b4];
    //u8 _019[0x248 - 0x019];
};
static_assert(sizeof(KartItem) == 0x248);

} // namespace Item
