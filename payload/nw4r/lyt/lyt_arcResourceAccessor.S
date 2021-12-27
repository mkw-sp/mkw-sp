#include <Common.S>

// Insert archives at the back to match the order from mkw's MultiDvdArchive
PATCH_REPLACE_START(MultiArcResourceAccessor_Attach, 0x10)
    lwz r0, 0x8 (r3)
PATCH_REPLACE_END(MultiArcResourceAccessor_Attach, 0x10)
