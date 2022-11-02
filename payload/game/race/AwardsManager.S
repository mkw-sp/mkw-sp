#include <Common.S>

// Fixes a bug causing Mii heads to appear on normal characters
PATCH_BL_START(AwardsManager_initPlayers, 0x258)
    // Check if it's a Mii character ID
    subi    r0, r15, 0x18
    li      r7, 0
    cmplwi  r0, 0x14
    bgtlr
    addi    r7, r10, 0x1C
    blr
PATCH_BL_END(AwardsManager_initPlayers, 0x258)
