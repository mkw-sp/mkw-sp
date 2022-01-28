#include <Common.S>

PATCH_B_START(RaceMenuPage_onButtonFront, 0x328)
    addi r3, r6, 0x20 + 0xbf0
    bl RaceConfigScenario_resetGhostPlayerTypes

    lwz r12, 0x0 (r28) // Original instruction
    b RaceMenuPage_onButtonFront + 0x32c
PATCH_B_END(RaceMenuPage_onButtonFront, 0x328)
