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
