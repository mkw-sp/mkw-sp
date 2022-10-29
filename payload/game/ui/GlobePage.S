#include <Common.S>

PATCH_REPLACE_START(GlobePage_onInit, 0x7c)
    // Update for NetManager removal
    nop
PATCH_REPLACE_END(GlobePage_onInit, 0x7c)
