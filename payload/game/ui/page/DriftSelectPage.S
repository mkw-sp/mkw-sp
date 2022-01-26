#include <Common.S>

PATCH_BL_START(DriftSelectPage_onButtonFront, 0x48)
    lwz r5, 0x240 (r31) // Original instruction

    // Check that the button is a drift mode one
    cmplwi r5, 0x2
    bgelr

    lis r3, s_saveManager@ha
    lwz r3, s_saveManager@l (r3)
    mr r4, r5
    bl SaveManager_setDriftMode

    lwz r5, 0x240 (r31) // Original instruction again
    b DriftSelectPage_onButtonFront + 0x4c
PATCH_BL_END(DriftSelectPage_onButtonFront, 0x48)
