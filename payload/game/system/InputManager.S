#include <Common.S>

PATCH_BL_START(WiiPad_processClassic, 0x1e8)
    ori r6, r6, 0x1 // Original instruction

    // Check that 200cc is enabled
    lis r8, speedModIsEnabled@ha
    lbz r8, speedModIsEnabled@l (r8)
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
    lis r4, speedModIsEnabled@ha
    lbz r4, speedModIsEnabled@l (r4)
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



PATCH_B_START(InputManager_reset, 0x54)
    mr r3, r30
    bl InputManager_resetMultiGhostProxies

    mr r3, r30 // Original instruction
    b InputManager_reset + 0x58
PATCH_B_END(InputManager_reset, 0x54)

PATCH_B_START(InputManager_calcPads, 0xd0)
    mr r3, r30
    bl InputManager_calcMultiGhostPads

    li r27, 0x0 // Original instruction
    b InputManager_calcPads + 0xd4
PATCH_B_END(InputManager_calcPads, 0xd0)

PATCH_B_START(InputManager_calc, 0x6c)
    mr r3, r29
    lbz r4, 0x4154 (r29)
    bl InputManager_calcMultiGhostProxies

    lwz r12, 0x15b4 (r29) // Original instruction
    b InputManager_calc + 0x70
PATCH_B_END(InputManager_calc, 0x6c)
