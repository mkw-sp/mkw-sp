#include <Common.S>

PATCH_BL_START(CourseSelectPage_onActivate, 0xd0)
    // Adjust engine classes to handle the removal of 50cc and the addition of 200cc
    addi r3, r3, 0xd52

    // Check that 200cc is enabled
    lis r5, vsSpeedModIsEnabled@ha
    lbz r5, vsSpeedModIsEnabled@l (r5)
    cmpwi r5, 0x0
    beqlr

    // Set the message id for 200cc
    li r0, 0xd56
    b CourseSelectPage_onActivate + 0xe8
PATCH_BL_END(CourseSelectPage_onActivate, 0xd0)
