#include <Common.S>

PATCH_BL_START(KartObject_calcPass1, 0x28)
    lwz r3, 0x18 (r30) // Original instruction

    lwz r4, 0x74 (r3)
    cmpwi r4, 0x0
    beqlr

    // Get the player id
    lwz r5, 0x0 (r30)
    lwz r5, 0x0 (r5)
    lbz r5, 0x10 (r5)

    lis r6, s_playerDrawPriorities@h
    ori r6, r6, s_playerDrawPriorities@l
    lbzx r5, r6, r5

    lwz r6, 0x4 (r4)
    lwz r6, 0x14 (r6)
    lwz r6, 0x0 (r6)
    stb r5, 0xd0 (r6)

    lwz r6, 0x4 (r4)
    lwz r6, 0x18 (r6)
    lwz r6, 0x0 (r6)
    stb r5, 0xd0 (r6)

    lwz r6, 0x8 (r4)
    lwz r6, 0x14 (r6)
    lwz r6, 0x0 (r6)
    stb r5, 0xd0 (r6)

    lwz r6, 0x8 (r4)
    lwz r6, 0x18 (r6)
    lwz r6, 0x0 (r6)
    stb r5, 0xd0 (r6)

    b KartObject_calcPass1 + 0x2c
PATCH_BL_END(KartObject_calcPass1, 0x28)



PATCH_B_START(KartObject_calcPass1, 0x144)
    // Hide the shadow
    li r4, 0x0
    stw r4, 0x4 (r3)

    mr r30, r3 // Original instruction
    b KartObject_calcPass1 + 0x148
PATCH_B_END(KartObject_calcPass1, 0x144)
