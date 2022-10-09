#include <Common.S>

PATCH_REPLACE_START(GlobePage_onInit, 0x7c)
    nop
PATCH_REPLACE_END(GlobePage_onInit, 0x7c)
