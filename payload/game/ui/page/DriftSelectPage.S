#include "../SectionId.S"
#include <Common.S>

// Initialize the previous page id for both "Change Course" and "Change Ghost Data"

PATCH_BL_START(DriftSelectPage_onInit, 0x98)
    cmpwi r0, 0x4a // Original instruction
    beqlr

    cmpwi r0, SingleChangeGhostData
    b DriftSelectPage_onInit + 0x9c
PATCH_BL_END(DriftSelectPage_onInit, 0x98)



PATCH_BL_START(DriftSelectPage_onButtonFront, 0x48)
    lwz r5, 0x240 (r31) // Original instruction

    // Check that the button is a drift mode one
    cmplwi r5, 0x2
    bgelr

    mr r3, r5
    bl SaveManager_SetDriftMode

    lwz r5, 0x240 (r31) // Original instruction again
    b DriftSelectPage_onButtonFront + 0x4c
PATCH_BL_END(DriftSelectPage_onButtonFront, 0x48)

PATCH_BL_START(DriftSelectPage_onButtonFront, 0x154)
cmpwi r0, 0x4f
b DriftSelectPage_onButtonFront+0x158
PATCH_BL_END(DriftSelectPage_onButtonFront, 0x154)