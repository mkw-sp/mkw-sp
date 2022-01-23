#include <Common.S>

PATCH_REPLACE_START(TimeAttackSplitsPage_ct, 0x20)
    // Initialize isReady to false
    li r0, 0x0
PATCH_REPLACE_END(TimeAttackSplitsPage_ct, 0x20)

PATCH_REPLACE_START(TimeAttackSplitsPage_vf_38, 0xf4)
    // Skip best time/lap animations and always try to save the ghost
    b 0x498
PATCH_REPLACE_END(TimeAttackSplitsPage_vf_38, 0xf4)

PATCH_REPLACE_START(TimeAttackSplitsPage_vf_38, 0x5f4)
    // Set the message id to "Saving ghost data..." first
    li r4, 0x3025
PATCH_REPLACE_END(TimeAttackSplitsPage_vf_38, 0x5f4)
