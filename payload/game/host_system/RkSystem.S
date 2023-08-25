#include <Common.S>

PATCH_B_START(RkSystem_main, 0x90)
    bl Rel_Run
    b RkSystem_main + 0x94
PATCH_B_END(RkSystem_main, 0x90)
