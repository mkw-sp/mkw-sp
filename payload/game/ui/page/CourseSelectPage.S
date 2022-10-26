#include "../SectionId.S"
#include "../PageId.S"
#include <Common.S>

PATCH_BL_START(CourseSelectPage_onInit, 0x20)
    lwz r0, 0x0 (r4) // Original instruction

    // Check that the section is "Change Ghost Data"
    cmpwi r0, SingleChangeGhostData
    bnelr

    // Set the previous page to cup select
    li r6, CupSelect
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



PATCH_B_START(CourseSelectPage_afterCalc, 0x44)
    b CourseSelectPage_afterCalc + 0x100
PATCH_B_END(CourseSelectPage_afterCalc, 0x44)



PATCH_B_START(CourseSelectPage_onSelect, 0xf4)
    bl _ZN2UI14VotingBackPage8InstanceEv
    mr r4, r31
    bl _ZN2UI14VotingBackPage12setLocalVoteEi
    li r4, 1
    stb r4, 0x136(r3)
    mr r3, r30
    bl PushButton_getDelay
    mr r3, r29
    li r4, 0
    bl Page_startReplace
    b CourseSelectPage_onSelect + 0x170
PATCH_B_END(CourseSelectPage_onSelect, 0xf4)
