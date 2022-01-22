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
