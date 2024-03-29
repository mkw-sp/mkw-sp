#include <Common.S>

PATCH_B_START(KartMove_calcAcceleration, 0x284)
    // Check that 200cc is enabled
    lis r3, g_speedModIsEnabled@ha
    lbz r3, g_speedModIsEnabled@l (r3)
    cmpwi r3, 0x0
    beq .Lend

    // Check that the player is drifting
    clrlwi r3, r4, 28
    cmpwi r3, 0xf
    bne .Lend

    // Check that the brake drifting bit is set
    lis r3, s_raceManager@ha
    lwz r3, s_raceManager@l (r3)
    lwz r3, 0xc (r3)
    lwz r9, 0x0 (r29)
    lwz r9, 0x0 (r9)
    lbz r9, 0x10 (r9)
    slwi r9, r9, 2
    lwzx r3, r3, r9
    lwz r3, 0x48 (r3)
    lhz r3, 0x2c (r3)
    extrwi. r3, r3, 1, 27
    beq .Lend

    b KartMove_calcAcceleration + 0x324

.Lend:
    clrlwi. r3, r4, 31 // Original instruction
    b KartMove_calcAcceleration + 0x288
PATCH_B_END(KartMove_calcAcceleration, 0x284)

PATCH_B_START(KartMove_calcSpeed, 0x210)
    lis r3, g_speedModFactor@ha
    lfs f4, g_speedModFactor@l (r3)
    fmuls f0, f0, f4
    fmuls f0, f0, f2 // Original instruction
    b KartMove_calcSpeed + 0x214
PATCH_B_END(KartMove_calcSpeed, 0x210)

PATCH_B_START(KartMove_calcSpeed, 0x3a4)
    lis r6, g_speedModFactor@ha
    lfs f2, g_speedModFactor@l (r6)
    fmuls f0, f0, f2
    fcmpo cr0, f0, f28 // Original instruction
    b KartMove_calcSpeed + 0x3a8
PATCH_B_END(KartMove_calcSpeed, 0x3a4)

PATCH_B_START(KartMove_calcSpeed, 0x3c8)
    lis r6, g_speedModFactor@ha
    lfs f2, g_speedModFactor@l (r6)
    fmuls f0, f0, f2
    fcmpo cr0, f0, f28 // Original instruction
    b KartMove_calcSpeed + 0x3cc
PATCH_B_END(KartMove_calcSpeed, 0x3c8)

PATCH_B_START(KartMove_calcCannon, 0x370)
    lis r5, g_speedModReverseFactor@ha
    lfs f7, g_speedModReverseFactor@l (r5)
    fmuls f0, f0, f7
    stfs f0, 0x20 (r30)
    b KartMove_calcCannon + 0x374
PATCH_B_END(KartMove_calcCannon, 0x370)

PATCH_BL_START(KartMove_calcSpeed, 0x65c)
    lwz r0, 0x4 (r3) // Original instruction

    // Check that the player id is 0
    lwz r4, 0x0 (r29)
    lwz r4, 0x0 (r4)
    lbz r4, 0x10 (r4)
    cmpwi r4, 0x0
    bnelr

    extrwi. r4, r0, 1, 1
    beqlr

    andi. r4, r0, 0x60
    beqlr

    bl SPFooter_OnWallride

    lwz r3, 0x0 (r29)
    lwz r3, 0x4 (r3)
    lwz r0, 0x4 (r3) // Original instruction again
    b KartMove_calcSpeed + 0x660
PATCH_BL_END(KartMove_calcSpeed, 0x65c)
