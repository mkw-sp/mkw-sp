#include <Common.S>

PATCH_BL_START(KartState_calcCollisions, 0x7a8)
    lfs f0, 0x24 (r31) // Original instruction

    // Check that the player id is 0
    lwz r3, 0x18 (r20)
    lwz r3, 0x0 (r3)
    lwz r3, 0x0 (r3)
    lbz r3, 0x10 (r3)
    cmpwi r3, 0x0
    bnelr

    bl SPFooter_OnHWG

    lfs f0, 0x24 (r31) // Original instruction again
    b KartState_calcCollisions + 0x7ac
PATCH_BL_END(KartState_calcCollisions, 0x7a8)
