#include <Common.S>

.global thunk_RaceConfigScenario_initGhostController
thunk_RaceConfigScenario_initGhostController:
    mflr r21

    mr r3, r24
    mr r4, r26
    bl RaceConfigScenario_initGhostController
    addi r27, r27, 0x1

    addi r21, r21, 0xa4
    mtlr r21
    blr
.size thunk_RaceConfigScenario_initGhostController, . - thunk_RaceConfigScenario_initGhostController

PATCH_BL(RaceConfigScenario_initControllers + 0xd8, thunk_RaceConfigScenario_initGhostController)
