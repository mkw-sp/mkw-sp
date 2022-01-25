#include <Common.S>

PATCH_B_START(CtrlRaceTime_calcSelf, 0x34)
    stb r0, 0x1b6 (r31) // Original instruction

    mr r3, r30
    bl RaceManager_getPlayerMaxLap
    stw r3, 0x1c0 (r31)

    b CtrlRaceTime_calcSelf + 0x38
PATCH_B_END(CtrlRaceTime_calcSelf, 0x34)

PATCH_BL_START(CtrlRaceTime_calcSelf, 0x74)
    subi r0, r3, 0x6a // Original instruction

    // Check that the game mode is offline vs
    lis r4, s_raceConfig@ha
    lwz r4, s_raceConfig@l (r4)
    lwz r5, 0x20 + 0xb50 (r4)
    cmpwi r5, 0x1
    bnelr

    // Check that there are at least 2 local players
    lwz r4, 0x20 + 0x8 + 0xf0 + 0x10 (r4)
    cmpwi r4, 0x0
    bnelr

    // Pretend that we are in live view, disabling the lap end animation
    li r3, 0x6a

    subi r0, r3, 0x6a // Original instruction again
    b CtrlRaceTime_calcSelf + 0x78
PATCH_BL_END(CtrlRaceTime_calcSelf, 0x74)
