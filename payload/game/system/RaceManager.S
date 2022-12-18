#include <Common.S>

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

    bl SPFooter_OnUltraShortcut

    mr r6, r28
    mr r5, r27
    mr r4, r25
    lwz r3, 0x38 (r5)
    lhz r0, 0x1a (r3) // Original instruction again
    b RaceManagerPlayer_calcCheckPoint + 0x324
PATCH_BL_END(RaceManagerPlayer_calcCheckPoint, 0x320)
