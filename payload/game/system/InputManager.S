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

    // Set the break drifting bit
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

    // Set the break drifting bit
    ori r8, r8, 0x10
    blr
PATCH_BL_END(GcPad_process, 0xec)



.global thunk_InputManager_resetMultiGhostProxies
thunk_InputManager_resetMultiGhostProxies:
    mflr r31

    mr r3, r30
    bl InputManager_resetMultiGhostProxies

    mr r3, r30 # Original instruction

    mtlr r31
    blr
.size thunk_InputManager_resetMultiGhostProxies, . - thunk_InputManager_resetMultiGhostProxies

PATCH_BL(InputManager_reset + 0x54, thunk_InputManager_resetMultiGhostProxies)

.global thunk_InputManager_calcMultiGhostPads
thunk_InputManager_calcMultiGhostPads:
    mflr r31

    mr r3, r30
    bl InputManager_calcMultiGhostPads

    li r27, 0x0 # Original instruction

    mtlr r31
    blr
.size thunk_InputManager_calcMultiGhostPads, . - thunk_InputManager_calcMultiGhostPads

PATCH_BL(InputManager_calcPads + 0xd0, thunk_InputManager_calcMultiGhostPads)

.global thunk_InputManager_calcMultiGhostProxies
thunk_InputManager_calcMultiGhostProxies:
    mflr r30

    mr r3, r29
    lbz r4, 0x4154 (r29)
    bl InputManager_calcMultiGhostProxies

    lwz r12, 0x15b4 (r29) # Original instruction

    mtlr r30
    blr
.size thunk_InputManager_calcMultiGhostProxies, . - thunk_InputManager_calcMultiGhostProxies

PATCH_BL(InputManager_calc + 0x6c, thunk_InputManager_calcMultiGhostProxies)
