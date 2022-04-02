#include <Common.S>

// Remove the call to ClearArena which would otherwise wipe the payload

PATCH_REPLACE_START(OSInit, 0x2cc)
    nop
PATCH_REPLACE_END(OSInit, 0x2cc)
