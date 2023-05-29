#include <Common.S>

// Do not attempt to load Staff Ghost Data
PATCH_REPLACE_START(RankingGraphControlTimeAttack_initHistogramTimes, 0x920)
    b   0xF4
PATCH_REPLACE_END(RankingGraphControlTimeAttack_initHistogramTimes, 0x920)
