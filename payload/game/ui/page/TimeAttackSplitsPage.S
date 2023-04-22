#include <Common.S>

PATCH_REPLACE_START(TimeAttackSplitsPage_beforeInAnim, 0xf4)
    // Skip best time/lap animations and always try to save the ghost
    b 0x498
PATCH_REPLACE_END(TimeAttackSplitsPage_beforeInAnim, 0xf4)

PATCH_REPLACE_START(TimeAttackSplitsPage_beforeInAnim, 0x5f4)
    // Set the message id to "Saving ghost data..." first
    li r4, 10047
PATCH_REPLACE_END(TimeAttackSplitsPage_beforeInAnim, 0x5f4)
