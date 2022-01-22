#include <Common.S>

PATCH_B_START(RaceConfigScenario_initPads, 0xd8)
    mr r3, r24
    mr r4, r26
    bl RaceConfigScenario_initGhostPad
    b RaceConfigScenario_initPads + 0x168
PATCH_B_END(RaceConfigScenario_initPads, 0xd8)
