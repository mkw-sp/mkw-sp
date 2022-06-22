#include <Common.S>

// Remove the check for the size to be a multiple of 32, since our slab allocator guarantees that.

PATCH_REPLACE_START(RecvFrom, 0xa4)
    b 0x2c
PATCH_REPLACE_END(RecvFrom, 0xa4)

PATCH_REPLACE_START(SendTo, 0x8c)
    b 0x2c
PATCH_REPLACE_END(SendTo, 0x8c)
