#include <Common.S>

// Pass second argument to KartModel::onIndirectHitSent

PATCH_B_START(KartCollide_checkItem, 0x544)
    mr r3, r17 // Original instruction
    clrlwi r4, r24, 24
    b KartCollide_checkItem + 0x548
PATCH_B_END(KartCollide_checkItem, 0x544)
