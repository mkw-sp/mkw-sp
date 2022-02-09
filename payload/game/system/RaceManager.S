#include <Common.S>

PATCH_B_START(RaceManagerPlayer_ct, 0x298)
    mr r3, r30
    bl RaceManagerPlayer_setMultiGhostPadProxy

    mr r3, r30 // Original instruction
    b RaceManagerPlayer_ct + 0x29c
PATCH_B_END(RaceManagerPlayer_ct, 0x298)



PATCH_BL_START(RaceManagerPlayer_endRace, 0x12c)
    lwz r0, 0x14 (r1) // Original instruction

    // Check that the player is a ghost
    lis r3, s_raceConfig@ha
    lwz r3, s_raceConfig@l (r3)
    lbz r4, 0x8 (r31)
    mulli r4, r4, 0xf0
    add r3, r3, r4
    lwz r3, 0x20 + 0x8 + 0x10 (r3)
    cmpwi r3, 0x3
    bnelr

    lwz r3, 0x48 (r31)
    bl GhostPadProxy_end

    lwz r0, 0x14 (r1) // Original instruction again
    b RaceManagerPlayer_endRace + 0x130
PATCH_BL_END(RaceManagerPlayer_endRace, 0x12c)



PATCH_BL_START(RaceManagerPlayer_calcCheckPoint, 0x320)
    lhz r0, 0x1a (r3) // Original instruction

    // Check that the player id is 0
    lbz r3, 0x8 (r26)
    cmpwi r3, 0x0
    bnelr

    lbz r3, 0x1c (r29)
    cmplwi r3, 0x1
    blelr

    mr r25, r4
    mr r27, r5
    mr r28, r6

    bl SpFooter_onUltraShortcut

    mr r6, r28
    mr r5, r27
    mr r4, r25
    lwz r3, 0x38 (r5)
    lhz r0, 0x1a (r3) // Original instruction again
    b RaceManagerPlayer_calcCheckPoint + 0x324
PATCH_BL_END(RaceManagerPlayer_calcCheckPoint, 0x320)
