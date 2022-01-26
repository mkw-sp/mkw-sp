#include <Common.S>

PATCH_B_START(DvdArchive_decompress, 0x28)
    bl Yaz_getSize
    b DvdArchive_decompress + 0x2c
PATCH_B_END(DvdArchive_decompress, 0x28)

PATCH_B_START(DvdArchive_decompress, 0x58)
    lwz r5, 0x18 (r28)
    mr r6, r31
    bl Yaz_decode
    b DvdArchive_decompress + 0x5c
PATCH_B_END(DvdArchive_decompress, 0x58)

PATCH_B_START(DvdArchive_loadOther, 0x30)
    // Copy the size, which is needed for decompression
    lwz r6, 0x18 (r4)
    stw r6, 0x18 (r3)

    lwz r4, 0x14 (r4) // Original instruction
    b DvdArchive_loadOther + 0x34
PATCH_B_END(DvdArchive_loadOther, 0x30)
