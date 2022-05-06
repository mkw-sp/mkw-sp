#include <Common.S>

PATCH_REPLACE_START(ARCOpen, 0x248)
    // Remove spammy warning when no file is found
    nop
PATCH_REPLACE_END(ARCOpen, 0x248)
