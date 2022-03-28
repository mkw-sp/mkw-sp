#include <Common.S>

// Always hide the player

PATCH_REPLACE_START(KartBlink_calc, 0x28)
    li r4, 0x0
PATCH_REPLACE_END(KartBlink_calc, 0x28)

PATCH_REPLACE_START(KartBlink_calc, 0x6c)
    li r0, 0x0
PATCH_REPLACE_END(KartBlink_calc, 0x6c)
