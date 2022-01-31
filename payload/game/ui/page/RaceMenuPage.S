#include <Common.S>

PATCH_B_START(RaceMenuPage_onButtonFront, 0x2e8)
    lwz r3, s_raceConfig@l (r31)
    addi r3, r3, 0x20 + 0xbf0
    bl RaceConfigScenario_resetGhostPlayerTypes

    lwz r4, s_raceConfig@l (r31) // Original instruction
    b RaceMenuPage_onButtonFront + 0x2ec
PATCH_B_END(RaceMenuPage_onButtonFront, 0x2e8)
