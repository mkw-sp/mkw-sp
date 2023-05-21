#include <Common.S>

.section .rodata
.Lfloats:
    .float 0.3
    .float 1.0
.previous

PATCH_BL_START(KartSub_calcPass0, 0x310)
    stfs f0, 0x148 (r4) // Original instruction

    // Check that 200cc is enabled
    lis r5, g_speedModIsEnabled@ha
    lbz r5, g_speedModIsEnabled@l (r5)
    cmpwi r5, 0x0
    beqlr

    // Get the airtime
    lwz r5, 0x1c (r29)
    lwz r6, 0x1c (r5)

    // Check that it is at least 2
    cmpwi r6, 2
    bltlr

    // Check if the player is hopping
    lbz r7, 0x96 (r5)
    cmpwi r7, 0x0
    beq .LnotHopping0

    // If so, check that the airtime is at least 19
    cmpwi r6, 19
    bltlr

.LnotHopping0:
    // Get the stick y value
    lis r6, s_raceManager@ha
    lwz r6, s_raceManager@l (r6)
    lwz r6, 0xc (r6)
    lwz r5, 0x18 (r5)
    lwz r5, 0x0 (r5)
    lwz r5, 0x0 (r5)
    lbz r5, 0x10 (r5)
    slwi r5, r5, 2
    lwzx r6, r6, r5
    lwz r6, 0x48 (r6)
    lfs f2, 0x34 (r6)

    fabs f3, f2
    fadds f2, f2, f3 // stickY <= 0.0f ? 0.0f : 2.0f * stickY
    lis r5, .Lfloats@ha
    lfs f3, .Lfloats@l + 0x0 (r5)
    lfs f4, .Lfloats@l + 0x4 (r5)
    fmadds f2, f2, f3, f4
    fmuls f0, f0, f2
    stfs f0, 0x148 (r4) // Original instruction again, with the modified value
    blr
PATCH_BL_END(KartSub_calcPass0, 0x310)

PATCH_BL_START(KartSub_calcPass1, 0x694)
    mr r3, r28 // Original instruction

    // Check that 200cc is enabled
    lis r4, g_speedModIsEnabled@ha
    lbz r4, g_speedModIsEnabled@l (r4)
    cmpwi r4, 0x0
    beqlr

    // Get the airtime
    lwz r4, 0x1c (r28)
    lwz r5, 0x1c (r4)

    // Check if the player is on the ground
    cmpwi r5, 0x0
    bne .LnotFirstFrame

    // If so, store whether the player is starting a hop
    lwz r6, 0x4 (r4)
    extrwi r6, r6, 1, 24
    stb r6, 0x96 (r4)

.LnotFirstFrame:
    // Check that the airtime is at least 2
    cmpwi r5, 2
    bltlr

    // Check if the player is hopping
    lbz r6, 0x96 (r4)
    cmpwi r6, 0x0
    beq .LnotHopping1

    // If so, check that the airtime is at least 19
    cmpwi r5, 19
    bltlr

.LnotHopping1:
    // Get the stick y value
    lis r5, s_raceManager@ha
    lwz r5, s_raceManager@l (r5)
    lwz r5, 0xc (r5)
    lwz r4, 0x18 (r4)
    lwz r4, 0x0 (r4)
    lwz r4, 0x0 (r4)
    lbz r4, 0x10 (r4)
    slwi r4, r4, 2
    lwzx r5, r5, r4
    lwz r5, 0x48 (r5)
    lfs f2, 0x34 (r5)

    fabs f3, f2
    fadds f2, f2, f3 // stickY <= 0.0f ? 0.0f : 2.0f * stickY
    lis r4, .Lfloats@ha
    lfs f3, .Lfloats@l + 0x0 (r4)
    lfs f4, .Lfloats@l + 0x4 (r4)
    fmadds f2, f2, f3, f4
    fmuls f0, f0, f2
    blr
PATCH_BL_END(KartSub_calcPass1, 0x694)



PATCH_BL_START(KartSub_calcTransforms, 0x20)
    lwz r12, 0xc (r3) // Original instruction

    lwz r4, 0x7c (r3)
    cmpwi r4, 0x0
    beqlr

    // Get the player id
    lwz r5, 0x0 (r29)
    lwz r5, 0x0 (r5)
    lbz r5, 0x10 (r5)

    lis r6, s_playerDrawPriorities@h
    ori r6, r6, s_playerDrawPriorities@l
    lbzx r5, r6, r5

    lwz r6, 0x14 (r4)
    lwz r6, 0x0 (r6)
    stb r5, 0xd0 (r6)

    lwz r6, 0x18 (r4)
    lwz r6, 0x0 (r6)
    stb r5, 0xd0 (r6)

    b KartSub_calcTransforms + 0x24
PATCH_BL_END(KartSub_calcTransforms, 0x20)

PATCH_BL_START(KartSub_calcTransforms, 0x44)
    lwz r12, 0xc (r3) // Original instruction

    lwz r4, 0x7c (r3)
    cmpwi r4, 0x0
    beqlr

    // Get the player id
    lwz r5, 0x0 (r29)
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

    b KartSub_calcTransforms + 0x48
PATCH_BL_END(KartSub_calcTransforms, 0x44)

PATCH_BL_START(KartSub_calcTransforms, 0xa4)
    lwz r12, 0xc (r30) // Original instruction

    lwz r4, 0x7c (r30)
    cmpwi r4, 0x0
    beqlr

    // Get the player id
    lwz r5, 0x0 (r29)
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

    b KartSub_calcTransforms + 0xa8
PATCH_BL_END(KartSub_calcTransforms, 0xa4)
