#include <Common.S>

PATCH_BL_START(CourseSelectPage_onInit, 0x20)
    lwz r0, 0x0 (r4) // Original instruction

    // Check that the section is "Change Ghost Data" (repurposed "Change Mission")
    cmpwi r0, 0x4d
    bnelr

    // Set the previous page to cup select
    li r6, 0x6e
    stw r6, 0x3ec (r3)

    b CourseSelectPage_onInit + 0x24
PATCH_BL_END(CourseSelectPage_onInit, 0x20)



PATCH_BL_START(CourseSelectPage_onActivate, 0xd0)
    addi r3, r3, 3411

    // Check that 200cc is enabled
    lis r5, vsSpeedModIsEnabled@ha
    lbz r5, vsSpeedModIsEnabled@l (r5)
    cmpwi r5, 0x0
    beqlr

    // Set the message id for 200cc
    li r0, 10072
    b CourseSelectPage_onActivate + 0xe8
PATCH_BL_END(CourseSelectPage_onActivate, 0xd0)
