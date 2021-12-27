#include <Common.S>

PATCH_B(CtrlRaceTime_getPlayerId, CtrlRaceBase_getPlayerId)

PATCH_INJECT_START(CtrlRaceTime_calcSelf, 0x34)
    stb r0, 0x1b6 (r31) # Original instruction

    mr r3, r30
    bl RaceManager_getPlayerMaxLap
    stw r3, 0x1c0 (r31)
PATCH_INJECT_END(CtrlRaceTime_calcSelf, 0x34)
