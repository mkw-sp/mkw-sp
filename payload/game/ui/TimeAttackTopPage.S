#include <Common.S>

PATCH_B_START(TimeAttackTopPage_onActivate, 0xa0)
    lwz r0, 0x94 (r30) // Original instruction

    // Send our request, which might complete immediately
    mr r3, r30
    bl GhostManagerPage_requestPopulate
    mr r3, r30
    bl GhostManagerPage_process

    lwz r0, 0x94 (r30) // Original instruction again
    b TimeAttackTopPage_onActivate + 0xa4
PATCH_B_END(TimeAttackTopPage_onActivate, 0xa0)
