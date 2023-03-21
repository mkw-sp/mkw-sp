#include <Common.S>

PATCH_BL_START(WipeControl_calcSelf, 0x3c)
    lis r31, s_kartObjectManager@ha // Original instruction

    // Check that the screen id is 0
    lwz r4, 0x174 (r28)
    cmpwi r4, 0x0
    bnelr

    lis r4, s_racePage@ha
    lwz r4, s_racePage@l (r4)
    cmpwi r4, 0x0
    beqlr

    lwz r3, 0x64 (r4)

    b WipeControl_calcSelf + 0x40
PATCH_BL_END(WipeControl_calcSelf, 0x3c)
