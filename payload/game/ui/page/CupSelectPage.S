#include "../SectionId.S"
#include <Common.S>

// Initialize the previous page id for both "Change Course" and "Change Ghost Data"

PATCH_BL_START(CupSelectPage_onInit, 0x84)
    cmpwi r0, 0x4a // Original instruction
    beqlr

    cmpwi r0, SingleChangeGhostData
    b CupSelectPage_onInit + 0x88
PATCH_BL_END(CupSelectPage_onInit, 0x84)



PATCH_BL_START(CupSelectPage_onActivate, 0x348)
    addi r3, r3, 3411 // Original instruction

    // Check that 200cc is enabled
    lis r5, vsSpeedModIsEnabled@ha
    lbz r5, vsSpeedModIsEnabled@l (r5)
    cmpwi r5, 0x0
    beqlr

    // Set the message id for 200cc
    li r0, 10072
    b CupSelectPage_onActivate + 0x360
PATCH_BL_END(CupSelectPage_onActivate, 0x348)



PATCH_B_START(CupSelectPage_afterCalc, 0x5c)
    b CupSelectPage_afterCalc + 0x140
PATCH_B_END(CupSelectPage_afterCalc, 0x5c)
