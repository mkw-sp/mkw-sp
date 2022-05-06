#include <Common.S>

PATCH_B_START(DvdArchive_loadOther, 0x30)
    // Copy the size, which is needed for decompression
    lwz r6, 0x18 (r4)
    stw r6, 0x18 (r3)

    lwz r4, 0x14 (r4) // Original instruction
    b DvdArchive_loadOther + 0x34
PATCH_B_END(DvdArchive_loadOther, 0x30)
