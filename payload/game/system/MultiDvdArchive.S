#include <Common.S>

PATCH_BL_START(MultiDvdArchive_exists, 0xb0)
    b DVDExConvertPathToEntrynum
PATCH_BL_END(MultiDvdArchive_exists, 0xb0)
