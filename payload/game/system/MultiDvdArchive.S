#include <Common.S>

PATCH_B_START(MultiDvdArchive_load, 0x5c)
    mr r3, r25
    mr r4, r26
    mr r5, r30
    addi r6, r1, 0x8
    bl MultiDvdArchive_formatShort

    b MultiDvdArchive_load + 0xa8
PATCH_B_END(MultiDvdArchive_load, 0x5c)
