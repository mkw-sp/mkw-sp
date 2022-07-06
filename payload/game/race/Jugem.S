#include <Common.S>

PATCH_BL_START(Jugem_80724430, 0x324)
    // Only play the SE_JUGEM_RESCUE sound if the Lakitu is visible
    lbz r5, 0x5e (r29)
    cmpwi r5, 0x0
    beqlr

    bctrl // Original instruction
    b Jugem_80724430 + 0x328
PATCH_BL_END(Jugem_80724430, 0x324)

PATCH_BL_START(Jugem_80724880, 0x67c)
    // Only play the SE_JUGEM_REVERSE sound if the Lakitu is visible
    lbz r5, 0x5e (r28)
    cmpwi r5, 0x0
    beqlr

    bctrl // Original instruction
    b Jugem_80724880 + 0x680
PATCH_BL_END(Jugem_80724880, 0x67c)
