#include <Common.S>

PATCH_BL_START(GhostManagerPage_process, 0x18)
    lwz r0, 0x94 (r3) // Original instruction

    // Check that the current request is populate
    cmpwi r0, 0xe
    bnelr

    bl GhostManagerPage_processPopulate

    b GhostManagerPage_process + 0x334
PATCH_BL_END(GhostManagerPage_process, 0x18)

PATCH_BL_START(GhostManagerPage_process, 0x390)
    li r0, 0x0 // Original instruction

    // Check that the next request is populate
    cmpwi r3, 0xe
    bnelr

    mr r3, r31
    bl GhostManagerPage_dispatchPopulate

    li r0, 0x0 // Original instruction again
    b GhostManagerPage_process + 0x394
PATCH_BL_END(GhostManagerPage_process, 0x390)
