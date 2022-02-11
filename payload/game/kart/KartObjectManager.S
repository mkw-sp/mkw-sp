#include <Common.S>

PATCH_B_START(KartObjectManager_calc, 0x38)
    lwz r30, 0x20 (r3) // Original instruction

    bl KartObjectManager_beforeCalc

    b KartObjectManager_calc + 0x3c
PATCH_B_END(KartObjectManager_calc, 0x38)
