#include <Common.S>

PATCH_B_START(TitlePage_onInit, 0x16c)
    // Enable alternative backgrounds
    li r0, 0x1
    stb r0, 0x348 (r31)

    b TitlePage_onInit + 0x170
PATCH_B_END(TitlePage_onInit, 0x16c)

PATCH_B_START(TitlePage_onActivate, 0x68)
    mr r3, r30
    bl TitlePage_processGeckoWarning

    lwz r0, 0x14 (r1) // Original instruction
    b TitlePage_onActivate + 0x6c
PATCH_B_END(TitlePage_onActivate, 0x68)
