#include <Common.S>

PATCH_BL_START(DVDReadPrio, 0x84)
    lwz r6, 0x30 (r27) // Original instruction

    lwz r3, 0x8 (r27)
    cmpwi r3, -1
    bnelr

    mr r3, r27
    mr r4, r28
    mr r5, r29
    mr r6, r31
    mr r7, r30
    bl DVDExReadPrio

    b DVDReadPrio + 0x110
PATCH_BL_END(DVDReadPrio, 0x84)

PATCH_BL_START(DVDReadAsyncPrio, 0x88)
    lwz r6, 0x30 (r26) // Original instruction

    lwz r3, 0x8 (r26)
    cmpwi r3, -1
    bnelr

    mr r3, r26
    mr r4, r27
    mr r5, r28
    mr r6, r29
    mr r7, r30
    mr r8, r31
    bl DVDExReadAsyncPrio

    b DVDReadAsyncPrio + 0xb4
PATCH_BL_END(DVDReadAsyncPrio, 0x88)

PATCH_BL_START(DVDClose, 0x8)
    stw r0, 0x14 (r1) // Original instruction

    lwz r4, 0x8 (r3)
    cmpwi r4, -1
    bnelr

    bl DVDExClose

    b DVDClose + 0x10
PATCH_BL_END(DVDClose, 0x8)

PATCH_B_START(DVDCancelAsync, 0x0)
    mr r5, r3

    lwz r6, 0x8 (r3)
    cmpwi r6, -1
    li r3, 1
    beqlr

    mr r3, r5
    stwu r1, -0x20 (r1)
    b DVDCancelAsync + 0x4
PATCH_B_END(DVDCancelAsync, 0x0)

PATCH_B_START(DVDCancel, 0x0)
    mr r4, r3

    lwz r5, 0x8 (r3)
    cmpwi r5, -1
    li r3, 1
    beqlr

    mr r3, r4
    stwu r1, -0x10 (r1)
    b DVDCancel + 0x4
PATCH_B_END(DVDCancel, 0x0)
