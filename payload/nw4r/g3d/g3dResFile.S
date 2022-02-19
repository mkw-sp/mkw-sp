#include <Common.S>

PATCH_B_START(ResFile_CheckRevision, 0x74c)
    // r3 = version
    li r4, ('S' << 8) | 'R' // SRT0 error
    mr r5, r30 // Index
    li r6, 0
    b InvalidRevisionFail
    // Unreachable
PATCH_B_END(ResFile_CheckRevision, 0x74c)

PATCH_B_START(ResFile_CheckRevision, 0x948)
    cmpwi r3, 1
    beqlr // If it's valid, do nothing

    // Failsafe for unlikely revision failures
    li r3, -1
    li r4, 0
    li r5, -1
    li r6, 0
    b InvalidRevisionFail
    // Unreachable
PATCH_B_END(ResFile_CheckRevision, 0x948)
