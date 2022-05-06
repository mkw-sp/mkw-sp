#include <Common.S>

PATCH_BL_START(HomeButton_ct, 0x6c)
    mr r5, r26 // Original instruction

    // Check that the region is not K
    lis r4, 0x8000
    lhz r4, 0x620a (r4)
    cmpwi r4, 0x5511
    beqlr

    // Check that the language is unsupported
    cmpwi r27, 0x7
    bltlr

    // Load English instead
    li r27, 0x1

    b HomeButton_ct + 0x70
PATCH_BL_END(HomeButton_ct, 0x6c)
