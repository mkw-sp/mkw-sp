#pragma once

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

class ItemInventory {
public:
    REPLACE void setItem(u32 itemID);

    void REPLACED(resetItem)();
    REPLACE void resetItem();

    void REPLACED(resetHeldItem)();
    REPLACE void resetHeldItem();

    Items getItem();

    void setPressed(bool pressed);

    bool getPressed();

private:
    u8 _00[0x04 - 0x00];
    Items m_currentItemID;
    u32 m_currentItemCount;
    u8 _0c[0x28 - 0x0c];
    bool pressedLastFrame; // Added since field is unused
    u8 _2b[0x2c - 0x2b];
};
static_assert(sizeof(ItemInventory) == 0x2c);

} // namespace Item