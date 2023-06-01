#pragma once

#include <Common.hh>

namespace Item {

enum class ItemId {
    None = -0x01,
    Green = 0x00,
    Red = 0x01,
    Nana = 0x02,
    FIB = 0x03,
    Shroom = 0x04,
    TripShrooms = 0x05,
    Bomb = 0x06,
    Blue = 0x07,
    Shock = 0x08,
    Star = 0x09,
    Golden = 0x0a,
    Mega = 0x0b,
    Blooper = 0x0c,
    Pow = 0x0d,
    TC = 0x0e,
    Bill = 0x0f,
    TripGreens = 0x10,
    TripReds = 0x11,
    TripNanas = 0x12,
    Unused = 0x13,
    NoItem = 0x14,
};

class ItemInventory {
public:
    REPLACE void setItem(ItemId itemID);

    void REPLACED(resetItem)();
    REPLACE void resetItem();

    void REPLACED(resetHeldItem)();
    REPLACE void resetHeldItem();

    ItemId getCurrentItem() const;

    void setItemWheelPressed(bool pressed);

    bool getItemWheelPressed() const;

private:
    u8 _00[0x04 - 0x00];
    ItemId m_currentItemID;
    s32 m_currentItemCount;
    u8 _0c[0x29 - 0x0c];
    bool m_itemWheelPressed; // Added for item wheel. Field is unused
    u8 _2b[0x2c - 0x2b];
};
static_assert(sizeof(ItemInventory) == 0x2c);

} // namespace Item
