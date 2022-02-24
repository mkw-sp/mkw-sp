#include <Common.S>

PATCH_REPLACE_START(EGG_DvdFile_openEntrynum, 0x5c)
    // Remove useless call to DVDGetCommandBlockStatus
    nop
PATCH_REPLACE_END(EGG_DvdFile_openEntrynum, 0x5c)

PATCH_BL_START(EGG_DvdFile_openPath, 0x18)
    b DVDExConvertPathToEntrynum
PATCH_BL_END(EGG_DvdFile_openPath, 0x18)
