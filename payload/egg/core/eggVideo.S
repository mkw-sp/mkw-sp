#include <Common.S>

PATCH_BL_START(EGG_Video_configure, 0x68)
    mr r3, r31 // Original instruction

    // Check that the mode is progressive
    lwz r4, 0x14 (r3)
    cmpwi r4, 0x0
    bnelr

    // Change the filter to something sharper
    li r4, 0x0
    stb r4, 0x32 (r3)
    stb r4, 0x33 (r3)
    stb r4, 0x37 (r3)
    stb r4, 0x38 (r3)
    li r4, 0x15
    stb r4, 0x34 (r3)
    stb r4, 0x36 (r3)
    li r4, 0x16
    stb r4, 0x35 (r3)

    b EGG_Video_configure + 0x6c
PATCH_BL_END(EGG_Video_configure, 0x68)
