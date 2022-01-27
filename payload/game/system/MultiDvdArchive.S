#include <Common.S>

PATCH_BL_START(MultiDvdArchive_load, 0x5c)
    // Check that the archive type is a file with short format
    mflr r3
    addi r3, r3, 0xac
    mtlr r3
    cmpwi r0, 0x3
    bnelr

    mr r3, r25
    mr r4, r26
    mr r5, r30
    addi r6, r1, 0x8
    bl MultiDvdArchive_formatShort

    b MultiDvdArchive_load + 0xa8
PATCH_BL_END(MultiDvdArchive_load, 0x5c)
