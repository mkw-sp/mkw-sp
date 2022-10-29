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
    // Temporary: remove timer and unnecessary page code
    // This offset will prove useful later when reimplementing timer
    b CupSelectPage_afterCalc + 0x140
PATCH_B_END(CupSelectPage_afterCalc, 0x5c)



PATCH_BL_START(CupSelectPage_onRefocus, 0x1c)
    // Submit vote for online
    lwz r29, 0x23d8(r3) // Original instruction

    bl _ZN2UI14VotingBackPage8InstanceEv
    bl _ZN2UI14VotingBackPage12getSubmittedEv
    cmpwi r3,0
    beq end

    lis r4, RaceCupSelectPage_zero@ha
    lfs f1, RaceCupSelectPage_zero@l (r4)
    li r0, -1
    stw r0, 0x3e8(r3)
    mr r3, r31
    li r4, 0
    bl Page_startReplace
end:
    b CupSelectPage_onRefocus + 0x1f8
PATCH_BL_END(CupSelectPage_onRefocus, 0x1c)
