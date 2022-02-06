#include <Common.S>

// Remove the controller explanation diagram

PATCH_REPLACE_START(RaceMenuPage_onInit, 0x9c)
    b 0x7c
PATCH_REPLACE_END(RaceMenuPage_onInit, 0x9c)

PATCH_REPLACE_START(RaceMenuPage_onInit, 0x2d0)
    b 0x840
PATCH_REPLACE_END(RaceMenuPage_onInit, 0x2d0)



PATCH_B_START(RaceMenuPage_onButtonFront, 0x2e8)
    lwz r3, s_raceConfig@l (r31)
    addi r3, r3, 0x20 + 0xbf0
    bl RaceConfigScenario_resetGhostPlayerTypes

    lwz r4, s_raceConfig@l (r31) // Original instruction
    b RaceMenuPage_onButtonFront + 0x2ec
PATCH_B_END(RaceMenuPage_onButtonFront, 0x2e8)



// We are replacing the "Change Mission" button with "Change Ghost Data"; get the current section
// id dynamically instead of hardcoding mission mode.

PATCH_B_START(RaceMenuPage_onButtonFront, 0x5c8)
    lis r3, s_sectionManager@ha
    lwz r3, s_sectionManager@l (r3)
    lwz r3, 0x0 (r3)
    lwz r3, 0x0 (r3)

    b RaceMenuPage_onButtonFront + 0x5cc
PATCH_B_END(RaceMenuPage_onButtonFront, 0x5c8)
