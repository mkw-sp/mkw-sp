#include <Common.S>

PATCH_B_START(CtrlRaceBase_getPlayerId, 0x24)
    // Check if the section id is ghost replay
    cmpwi r0, 0x34
    bne .Lend

    li r0, 0x6a // Pretend it's live view instead

.Lend:
    cmpwi r0, 0x6a // Original instruction

    b CtrlRaceBase_getPlayerId + 0x28
PATCH_B_END(CtrlRaceBase_getPlayerId, 0x24)
