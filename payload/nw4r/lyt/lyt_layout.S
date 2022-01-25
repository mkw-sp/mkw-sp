#include <Common.S>

PATCH_BL_START(Layout_CreateAnimTransform, 0x38)
    mtctr r12 # Original instruction

    # Check that there is a pat1 section
    lwz r5, 0x8 (r4)
    cmpwi r5, 0x0
    beqlr

    # Check if groups are explicitly specified
    lhz r5, 0xa (r5)
    cmpwi r5, 0x0
    beqlr

    mflr r0
    stw r0, 0x10 (r1)
    mr r31, r4

    bctrl

    cmpwi r3, 0x0
    lwz r0, 0x10 (r1)
    mtlr r0
    beqlr

    stw r28, 0xc (r1)
    stw r27, 0x8 (r1)

    mr r30, r3
    li r28, 0x0 # Loop counter
    li r27, 0x0 # Number of animations

.Lloop:
    lwz r3, 0x8 (r31)
    lhz r4, 0xa (r3)
    cmpw r28, r4
    bge .Lout

    lwz r4, 0x10 (r3)
    add r3, r3, r4
    mulli r4, r28, 0x14
    add r4, r3, r4

    lwz r3, 0x4 (r30)
    lwz r3, -0x8 + 0x14 (r3)
    bl GroupContainer_FindGroupByName

    mr r4, r3
    mr r3, r31
    lwz r5, 0x8 (r3)
    lbz r5, 0x18 (r5)
    clrlwi r5, r5, 31
    bl AnimResource_CalcAnimationNum

    add r27, r27, r3

    addi r28, r28, 0x1
    b .Lloop

.Lout:
    mr r3, r30
    lwz r4, 0x4 (r31)
    mr r5, r29
    mr r6, r27
    lwz r12, 0x0 (r3)
    lwz r12, 0x10 (r12)
    mtctr r12

    lwz r27, 0x8 (r1)
    lwz r28, 0xc (r1)
    lwz r7, 0x10 (r1)
    addi r7, r7, 0x28
    mtlr r7
    bctr
PATCH_BL_END(Layout_CreateAnimTransform, 0x38)
