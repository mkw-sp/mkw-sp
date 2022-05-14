#include <Common.S>

// Do not initialize the course cache here

PATCH_REPLACE_START(RootScene_allocate, 0x44)
    b 0x10
PATCH_REPLACE_END(RootScene_allocate, 0x44)
