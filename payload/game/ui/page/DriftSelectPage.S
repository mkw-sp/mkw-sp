#include <Common.S>

// Initialize the previous page id for both "Change Course" and "Change Ghost Data"

PATCH_BL_START(DriftSelectPage_onInit, 0x98)
    cmpwi r0, 0x4a // Original instruction
    beqlr

    cmpwi r0, ChangeGhostData
    b DriftSelectPage_onInit + 0x9c
PATCH_BL_END(DriftSelectPage_onInit, 0x98)



PATCH_BL_START(DriftSelectPage_onButtonFront, 0x48)
    lwz r5, 0x240 (r31) // Original instruction

    // Check that the button is a drift mode one
    cmplwi r5, 0x2
    bgelr

    lis r3, s_saveManager@ha
    lwz r3, s_saveManager@l (r3)
    li r4, kSetting_DriftMode
    // r5 = value
    bl SaveManager_setSetting

    lwz r5, 0x240 (r31) // Original instruction again
    b DriftSelectPage_onButtonFront + 0x4c
PATCH_BL_END(DriftSelectPage_onButtonFront, 0x48)
