#include <Common.S>

PATCH_BL_START(Input_80745be4, 0xd0)
    ori r31, r31, 0x1 // Original instruction

    // Check that 200cc is enabled
    lis r6, g_speedModIsEnabled@ha
    lbz r6, g_speedModIsEnabled@l (r6)
    cmpwi r6, 0x0
    beqlr

    // Check that both 1 and B are pressed
    andi. r6, r3, 0x404
    cmpwi r6, 0x404
    bnelr

    // Set the brake drifting bit
    ori r31, r31, 0x10
    blr
PATCH_BL_END(Input_80745be4, 0xd0)

PATCH_BL_START(Input_80745de4, 0x18c)
    ori r0, r0, 0x1 // Original instruction

    // Check that 200cc is enabled
    lis r3, g_speedModIsEnabled@ha
    lbz r3, g_speedModIsEnabled@l (r3)
    cmpwi r3, 0x0
    beqlr

    // Check that both 1 and B are pressed
    andi. r3, r4, 0x600
    cmpwi r3, 0x600
    bnelr

    // Set the brake drifting bit
    ori r0, r0, 0x10
    blr
PATCH_BL_END(Input_80745de4, 0x18c)
