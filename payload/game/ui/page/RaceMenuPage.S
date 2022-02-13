#include <Common.S>

// Remove the controller explanation diagram

PATCH_REPLACE_START(RaceMenuPage_onInit, 0x9c)
    b 0x7c
PATCH_REPLACE_END(RaceMenuPage_onInit, 0x9c)

// Use our version of the button ID -> string array
PATCH_REPLACE_START(RaceMenuPage_onInit, 0x18c)
    lis r15, sButtonStrings@ha // +0x18c
    stw r3, 0x48(r28) // +0x190 Original instruction
    addi r15, r15, sButtonStrings@l // +0x194
PATCH_REPLACE_END(RaceMenuPage_onInit, 0x18c)

PATCH_REPLACE_START(RaceMenuPage_onInit, 0x2d0)
    b 0x840
PATCH_REPLACE_END(RaceMenuPage_onInit, 0x2d0)



// Do not show different messages when quitting from ghost watch.

PATCH_REPLACE_START(RaceMenuPage_onActivate, 0x404)
    b 0x30
PATCH_REPLACE_END(RaceMenuPage_onActivate, 0x404)



// Reset the replacement of the previous page when pressing "Continue" on the confirmation page.

PATCH_B_START(RaceMenuPage_onButtonFront, 0x128)
    lwz r3, 0x0 (r3) // Original instruction

    lwz r4, 0x58 (r28)
    slwi r4, r4, 0x2
    add r4, r3, r4
    lwz r4, 0x8 (r4)
    li r5, -0x1
    stw r5, 0xf0 (r4)

    b RaceMenuPage_onButtonFront + 0x12c
PATCH_B_END(RaceMenuPage_onButtonFront, 0x128)



// When quitting from ghost watch, go to the main menu.

PATCH_REPLACE_START(RaceMenuPage_onButtonFront, 0x224)
    nop
PATCH_REPLACE_END(RaceMenuPage_onButtonFront, 0x224)



// The game doesn't reset the player type of the ghosts, which can sometimes lead to an infinite
// loop if there is more than one. So do it here.

PATCH_B_START(RaceMenuPage_onButtonFront, 0x2e8)
    lwz r3, s_raceConfig@l (r31)
    addi r3, r3, 0x20 + 0xbf0
    bl RaceConfigScenario_resetGhostPlayerTypes

    lwz r4, s_raceConfig@l (r31) // Original instruction
    b RaceMenuPage_onButtonFront + 0x2ec
PATCH_B_END(RaceMenuPage_onButtonFront, 0x2e8)

PATCH_B_START(RaceMenuPage_onButtonFront, 0x4b8)
    lis r3, s_raceConfig@ha
    lwz r3, s_raceConfig@l (r3)
    addi r3, r3, 0x20 + 0xbf0
    bl RaceConfigScenario_resetGhostPlayerTypes

    lis r3, s_raceConfig@ha // Original instruction
    b RaceMenuPage_onButtonFront + 0x4bc
PATCH_B_END(RaceMenuPage_onButtonFront, 0x4b8)

PATCH_B_START(RaceMenuPage_onButtonFront, 0x4f8)
    lis r3, s_raceConfig@ha
    lwz r3, s_raceConfig@l (r3)
    addi r3, r3, 0x20 + 0xbf0
    bl RaceConfigScenario_resetGhostPlayerTypes

    lis r3, s_raceConfig@ha // Original instruction
    b RaceMenuPage_onButtonFront + 0x4fc
PATCH_B_END(RaceMenuPage_onButtonFront, 0x4f8)

PATCH_B_START(RaceMenuPage_onButtonFront, 0x594)
    lis r3, s_raceConfig@ha
    lwz r3, s_raceConfig@l (r3)
    addi r3, r3, 0x20 + 0xbf0
    bl RaceConfigScenario_resetGhostPlayerTypes

    lis r3, s_raceConfig@ha // Original instruction
    b RaceMenuPage_onButtonFront + 0x598
PATCH_B_END(RaceMenuPage_onButtonFront, 0x594)



// We are replacing the "Change Mission" button with "Change Ghost Data"; get the current section
// id dynamically instead of hardcoding mission mode.

PATCH_B_START(RaceMenuPage_onButtonFront, 0x5c8)
    lis r3, s_sectionManager@ha
    lwz r3, s_sectionManager@l (r3)
    lwz r3, 0x0 (r3)
    lwz r3, 0x0 (r3)

    b RaceMenuPage_onButtonFront + 0x5cc
PATCH_B_END(RaceMenuPage_onButtonFront, 0x5c8)

otherButtonId:
    mr r3, r28 // RaceMenuPage *this
    mr r4, r29 // PushButton *pushButton
    mr r5, r30 // u32 localPlayerId
    bl RaceMenuPage_onFrontOther

    cmpwi r3, 0
    beq unhandled
        b RaceMenuPage_onButtonFront + 0x1aec // Handled
    unhandled:
        // REL14 can't fit this
        b RaceMenuPage_onButtonFront + 0x1abc // Unhandled

PATCH_B_START(RaceMenuPage_onButtonFront, 0x1aac)
// buttonId != kPausePageButtonID_NotGoRanking
bne otherButtonId
// buttonId == kPausePageButtonID_NotGoRanking
b RaceMenuPage_onButtonFront + 0x1ab0
PATCH_B_END(RaceMenuPage_onButtonFront, 0x1aac)
