#include <Common.S>

.global DCInvalidateRange
DCInvalidateRange:
    // Return early if the size is zero
    cmpwi r4, 0
    beqlr

    // Setup the block count
    clrlwi r5, r3, 27
    add r4, r4, r5
    addi r4, r4, 0x1f
    srwi r4, r4, 5
    mtctr r4

    // Invalidate the blocks in a loop
.LloopDCI:
    dcbi 0, r3
    addi r3, r3, 0x20
    bdnz .LloopDCI

    // Wait until everything has completed
    sync

    blr
.size DCInvalidateRange, . - DCInvalidateRange

.global DCFlushRange
DCFlushRange:
    // Return early if the size is zero
    cmpwi r4, 0
    beqlr

    // Setup the block count
    clrlwi r5, r3, 27
    add r4, r4, r5
    addi r4, r4, 0x1f
    srwi r4, r4, 5
    mtctr r4

    // Flush the blocks in a loop
.LloopDCF:
    dcbf 0, r3
    addi r3, r3, 0x20
    bdnz .LloopDCF

    // Wait until everything has completed
    sync

    blr
.size DCFlushRange, . - DCFlushRange

.global ICInvalidateRange
ICInvalidateRange:
    // Return early if the size is zero
    cmpwi r4, 0
    beqlr

    // Setup the block count
    clrlwi r5, r3, 27
    add r4, r4, r5
    addi r4, r4, 0x1f
    rlwinm r4, r4, 27, 5, 31
    mtctr r4

    // Invalidate the blocks in a loop
.LloopICI:
    icbi 0, r3
    addi r3, r3, 0x20
    bdnz .LloopICI

    // Wait until everything has completed
    sync

    // Discard the already fetched instructions
    isync

    blr
.size ICInvalidateRange, . - ICInvalidateRange
