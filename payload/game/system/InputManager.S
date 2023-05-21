#include <Common.S>

PATCH_BL_START(WiiPad_processClassic, 0x1e8)
    ori r6, r6, 0x1 // Original instruction

    // Check that 200cc is enabled
    lis r8, g_speedModIsEnabled@ha
    lbz r8, g_speedModIsEnabled@l (r8)
    cmpwi r8, 0x0
    beqlr

    // Check that both B and R are pressed
    andi. r8, r3, 0x240
    cmpwi r8, 0x240
    bnelr

    // Set the brake drifting bit
    ori r6, r6, 0x10
    blr
PATCH_BL_END(WiiPad_processClassic, 0x1e8)

PATCH_BL_START(GcPad_process, 0xec)
    ori r8, r8, 0x1 // Original instruction

    // Check that 200cc is enabled
    lis r4, g_speedModIsEnabled@ha
    lbz r4, g_speedModIsEnabled@l (r4)
    cmpwi r4, 0x0
    beqlr

    // Check that both B and R are pressed
    andi. r4, r5, 0x220
    cmpwi r4, 0x220
    bnelr

    // Set the brake drifting bit
    ori r8, r8, 0x10
    blr
PATCH_BL_END(GcPad_process, 0xec)
