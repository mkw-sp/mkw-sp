#include <Common.S>

PATCH_B_START(ResFile_CheckRevision, 0x74c)
    // r3 = version
    li r4, ('S' << 8) | 'R' // SRT0 error
    mr r5, r30 // Index
    li r6, 0
    b InvalidRevisionFail
    // Unreachable
PATCH_B_END(ResFile_CheckRevision, 0x74c)

