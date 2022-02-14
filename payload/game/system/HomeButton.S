#include <Common.S>

PATCH_B_START(HomeButton_getFile, 0x3c)
    bl Yaz_getSize
    b HomeButton_getFile + 0x40
PATCH_B_END(HomeButton_getFile, 0x3c)

PATCH_B_START(HomeButton_getFile, 0x5c)
    lwz r5, 0x8 (r1)
    mr r6, r30
    bl Yaz_decode
    b HomeButton_getFile + 0x60
PATCH_B_END(HomeButton_getFile, 0x5c)

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
