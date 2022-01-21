#include <Common.S>

PATCH_BL_START(Snd_806fafb4, 0x44)
    extrwi r27, r0, 1, 29 // Original instruction

    // Check that 200cc is enabled
    lis r5, speedModIsEnabled@ha
    lbz r5, speedModIsEnabled@l (r5)
    cmpwi r5, 0x0
    beqlr

    // Check that the player is drifting
    clrlwi r5, r0, 27
    cmpwi r5, 0xf
    bnelr

    // Check that the break drifting bit is set
    lis r5, s_raceManager@ha
    lwz r5, s_raceManager@l (r5)
    lwz r5, 0xc (r5)
    lwz r6, 0x18 (r4)
    lwz r6, 0x0 (r6)
    lwz r6, 0x0 (r6)
    lbz r6, 0x10 (r6)
    slwi r6, r6, 2
    lwzx r5, r5, r6
    lwz r5, 0x48 (r5)
    lhz r5, 0x2c (r5)
    extrwi. r5, r5, 1, 27
    beqlr

    extrwi. r5, r0, 1, 0
    bnelr

    extrwi. r5, r0, 1, 11
    bnelr

    li r27, 0x0
    li r28, 0x1
    li r30, 0x0
    blr
PATCH_BL_END(Snd_806fafb4, 0x44)



// This function handles sound triggers. Normally they are only enabled for human players, except
// when the current section is a ghost replay one (0x32, 0x33, 0x34). This makes the game crash
// with multighost because the local player id can be -1 while the game assumes that it will be
// positive. To fix that this patch completely removes the exception (for now).
PATCH_REPLACE_START(Snd_80708b44, 0x28)
    blr
PATCH_REPLACE_END(Snd_80708b44, 0x28)
