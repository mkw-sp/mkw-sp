#include <Common.S>

// Replace DVDFastOpen with DVDOpen so we don't have to deal with fake entrynums for file
// replacement.

PATCH_BL_START(ut_DvdFileStream_ct, 0x70)
    b DVDOpen
PATCH_BL_END(ut_DvdFileStream_ct, 0x70)
