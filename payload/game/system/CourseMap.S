#include <Common.S>

PATCH_B_START(CourseMap_computeFinishTime, 0x38)
    stw r3, 0x18 (r1) // Backup the checkpoint

    bl CheckPoint_computeFinishTime // Run the normal computation
    lwz r4, 0x18 (r1) // Restore the checkpoint
    stw r3, 0x18 (r1) // Backup the milliseconds

    // Get the player id from the parent function and check that it is 0
    lbz r5, 0x8 (r29)
    cmpwi r5, 0x0
    bne .Lend

    addi r5, r1, 0x10 // Last position
    addi r6, r1, 0x8 // Position

    lfs f1, 0x10 (r4)
    lfs f2, 0x14 (r4)

    lfs f3, 0x0 (r5)
    lfs f4, 0x4 (r5)

    lfs f5, 0x8 (r4)
    fsubs f5, f5, f3
    fmuls f5, f5, f1

    lfs f6, 0xc (r4)
    fsubs f6, f6, f4
    fmuls f6, f6, f2

    fadds f7, f5, f6

    lfs f5, 0x0 (r6)
    fsubs f5, f5, f3
    fmuls f5, f5, f1

    lfs f6, 0x4 (r6)
    fsubs f6, f6, f4
    fmuls f6, f6, f2

    fadds f8, f5, f6

    fdivs f1, f7, f8

    lis r4, s_systemManager@ha
    lwz r4, s_systemManager@l (r4)
    lfs f2, 0x7c (r4)
    fmuls f1, f1, f2

    li r4, 0x0
    stw r4, -0x4 (r1)
    lis r4, 0x4330
    stw r4, -0x8 (r1)
    lfd f2, -0x8 (r1)

    stw r3, -0x4 (r1)
    lfd f3, -0x8 (r1)
    fsubs f2, f3, f2

    lbz r3, 0x26 (r29) // Get the lap from the parent function
    addi r3, r3, -1
    fsubs f1, f1, f2
    bl SPFooter_OnLapEnd

    lwz r3, 0x18 (r1) // Restore the milliseconds

.Lend:
    b CourseMap_computeFinishTime + 0x3c
PATCH_B_END(CourseMap_computeFinishTime, 0x38)
