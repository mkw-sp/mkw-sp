#include <Common.S>

PATCH_BL_START(KartBody_calcTransforms, 0x1f0)
    // Set arm transparency

    lwz r12, 0xc (r3) // Original instruction

    lwz r4, 0x7c (r3)

    // Get the player id
    lwz r5, 0x0 (r31)
    lwz r5, 0x0 (r5)
    lbz r5, 0x10 (r5)

    lis r6, s_playerDrawPriorities@h
    ori r6, r6, s_playerDrawPriorities@l
    lbzx r5, r6, r5

    lwz r6, 0x14 (r4)
    cmpwi r6, 0x0
    beqlr
    lwz r6, 0x0 (r6)
    stb r5, 0xd0 (r6)

    lwz r6, 0x18 (r4)
    lwz r6, 0x0 (r6)
    stb r5, 0xd0 (r6)

    b KartBody_calcTransforms + 0x1f4
PATCH_BL_END(KartBody_calcTransforms, 0x1f0)

PATCH_BL_START(BikeBody_calcTransforms, 0x1b0)
    // Set handle transparency

    lwz r12, 0x244 (r28) // Original instruction

    lwz r4, 0x7c (r3)

    // Get the player id
    lwz r5, 0x0 (r28)
    lwz r5, 0x0 (r5)
    lbz r5, 0x10 (r5)

    lis r6, s_playerDrawPriorities@h
    ori r6, r6, s_playerDrawPriorities@l
    lbzx r5, r6, r5

    lwz r6, 0x14 (r4)
    cmpwi r6, 0x0
    beqlr
    lwz r6, 0x0 (r6)
    stb r5, 0xd0 (r6)

    lwz r6, 0x18 (r4)
    lwz r6, 0x0 (r6)
    stb r5, 0xd0 (r6)

    b BikeBody_calcTransforms + 0x1b4
PATCH_BL_END(BikeBody_calcTransforms, 0x1b0)

PATCH_BL_START(BikeBody_calcTransforms, 0x250)
    // Set swingarm transparency

    lwz r12, 0x2d8 (r28) // Original instruction

    lwz r4, 0x7c (r3)

    // Get the player id
    lwz r5, 0x0 (r28)
    lwz r5, 0x0 (r5)
    lbz r5, 0x10 (r5)

    lis r6, s_playerDrawPriorities@h
    ori r6, r6, s_playerDrawPriorities@l
    lbzx r5, r6, r5

    lwz r6, 0x14 (r4)
    cmpwi r6, 0x0
    beqlr
    lwz r6, 0x0 (r6)
    stb r5, 0xd0 (r6)

    lwz r6, 0x18 (r4)
    lwz r6, 0x0 (r6)
    stb r5, 0xd0 (r6)

    b BikeBody_calcTransforms + 0x254
PATCH_BL_END(BikeBody_calcTransforms, 0x250)
