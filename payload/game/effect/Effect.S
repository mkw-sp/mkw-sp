#include <Common.S>

PATCH_B_START(Effect_80698028, 0xf24)
    bl Effect_806979f0 // Original instruction

    // Check that 200cc is enabled
    lis r3, speedModIsEnabled@ha
    lbz r3, speedModIsEnabled@l (r3)
    cmpwi r3, 0x0
    beq .LendKart

    lis r3, s_raceManager@ha
    lwz r3, s_raceManager@l (r3)
    lwz r3, 0xc (r3)
    lwz r4, 0x118 (r31)
    lwz r4, 0x0 (r4)
    lwz r5, 0x4 (r4)
    lwz r4, 0x18 (r5)
    lwz r4, 0x0 (r4)
    lwz r4, 0x0 (r4)
    lbz r4, 0x10 (r4)
    slwi r4, r4, 2
    lwzx r3, r3, r4
    lwz r3, 0x48 (r3)
    lhz r4, 0x2c (r3)
    extrwi. r4, r4, 1, 27
    beq .LnotBreakDriftingKart

    lwz r5, 0x4 (r5)
    clrlwi r4, r5, 27
    cmpwi r4, 0xf
    bne .LnotBreakDriftingKart

    extrwi. r4, r5, 1, 0
    bne .LnotBreakDriftingKart

    extrwi. r4, r5, 11, 1
    bne .LnotBreakDriftingKart

    li r4, 0x1
    stb r4, 0x15 (r3)

    lis r29, Effect_806a19d8@h
    ori r29, r29, Effect_806a19d8@l

    b .LcallFuncs

.LnotBreakDriftingKart:
    lbz r5, 0x15 (r3)

    cmpwi r5, 0x1
    bne .LendKart

    li r5, 0x0
    stb r5, 0x15 (r3)

    lis r29, Effect_806a1b68@h
    ori r29, r29, Effect_806a1b68@l

.LcallFuncs:
    mr r3, r31
    lwz r4, 0x530 (r31)
    li r5, 0x22
    li r6, 0x23
    li r7, 0x0
    mtctr r29
    bctrl

    mr r3, r31
    lwz r4, 0x530 (r31)
    li r5, 0x23
    li r6, 0x24
    li r7, 0x1
    mtctr r29
    bctrl

.LendKart:
    b Effect_80698028 + 0xf28
PATCH_B_END(Effect_80698028, 0xf24)

PATCH_B_START(Effect_80698f70, 0xd28)
    // Check that 200cc is enabled
    lis r3, speedModIsEnabled@ha
    lbz r3, speedModIsEnabled@l (r3)
    cmpwi r3, 0x0
    beq .LendBike

    lis r3, s_raceManager@ha
    lwz r3, s_raceManager@l (r3)
    lwz r3, 0xc (r3)
    lwz r4, 0x118 (r30)
    lwz r4, 0x0 (r4)
    lwz r5, 0x4 (r4)
    lwz r4, 0x18 (r5)
    lwz r4, 0x0 (r4)
    lwz r4, 0x0 (r4)
    lbz r4, 0x10 (r4)
    slwi r4, r4, 2
    lwzx r3, r3, r4
    lwz r3, 0x48 (r3)
    lhz r4, 0x2c (r3)
    extrwi. r4, r4, 1, 27
    beq .LnotBreakDriftingBike

    lwz r5, 0x4 (r5)
    clrlwi r4, r5, 27
    cmpwi r4, 0xf
    bne .LnotBreakDriftingBike

    extrwi. r4, r5, 1, 0
    bne .LnotBreakDriftingBike

    extrwi. r4, r5, 11, 1
    bne .LnotBreakDriftingBike

    li r4, 0x1
    stb r4, 0x15 (r3)

    lis r12, Effect_806a19d8@h
    ori r12, r12, Effect_806a19d8@l

    b .LcallFunc

.LnotBreakDriftingBike:
    lbz r5, 0x15 (r3)

    cmpwi r5, 0x1
    bne .LendBike

    li r5, 0x0
    stb r5, 0x15 (r3)

    lis r12, Effect_806a1b68@h
    ori r12, r12, Effect_806a1b68@l

.LcallFunc:
    mr r3, r30
    lwz r4, 0x614 (r30)
    li r5, 0x19
    li r6, 0x1a
    li r7, 0x1
    mtctr r12
    bctrl

.LendBike:
    addi r11, r1, 0xa0 // Original instruction
    b Effect_80698f70 + 0xd2c
PATCH_B_END(Effect_80698f70, 0xd28)
