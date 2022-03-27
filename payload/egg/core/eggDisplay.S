#include <Common.S>

PATCH_BL_START(EGG_Display_copyEFBToXFB, 0x74)
    cntlzw r0, r3 // Original instruction

    // Check that the mode is progressive
    lwz r5, 0x14 (r5)
    cmpwi r5, 0x0
    bnelr

    // Disable the copy filter
    li r0, 0x0

    b EGG_Display_copyEFBToXFB + 0x78
PATCH_BL_END(EGG_Display_copyEFBToXFB, 0x74)
