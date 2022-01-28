#include <Common.S>

// Skip ESRB for NTSC-U, no-op for other regions
PATCH_REPLACE_START(SectionManager_init, 0x110)
    cmpwi r0, -0x1
PATCH_REPLACE_END(SectionManager_init, 0x110)
