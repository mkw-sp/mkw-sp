#pragma once

#include "game/kart/KartObjectProxy.hh"
#include <Common.hh>

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
    None = 0xFF,
};

struct PlayerInventory {
    u8 _00[0x04 - 0x00];
    u32 currentItemID;
    u32 currentItemCount;
    u8 _0c[0x20 - 0x0c];
    u32 framesLeft;
    u32 somethingFrames;
    u8 _28[0x2c - 0x28];
};
static_assert(sizeof(PlayerInventory) == 0x2c);

// Tentative name
class KartItem : public Kart::KartObjectProxy {
public:
    void REPLACED(setItem)(u32 r4, u32 r5, u32 r6);
    REPLACE void setItem(u32 r4, u32 r5, u32 r6);

    void REPLACED(update)();
    REPLACE void update();

    void REPLACED(useGolden)();
    REPLACE void useGolden();

private:
    u8 _000[0x018 - 0x00c];
    u8 m_playerId;
    u8 _019[0x088 - 0x019];
    PlayerInventory m_inventory;
    u8 _0b4[0x248 - 0x0b4];
};
static_assert(sizeof(KartItem) == 0x248);

} // namespace Item
