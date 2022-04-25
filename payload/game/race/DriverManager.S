#include <Common.S>

PATCH_REPLACE_START(DriverManager_8078cfa4, 0x1a4)
    // Do not play the end of race animation for ghosts
    li r3, 0x4
PATCH_REPLACE_END(DriverManager_8078cfa4, 0x1a4)
