#include <Common.S>

// Fix the cheer sound/animation for our team mode

PATCH_B_START(KartCollide_checkItem, 0x254)
    clrlwi r3, r24, 24
    mr r4, r23
    bl RaceConfig_IsSameTeam
    cmpwi r3, 0x0
    b KartCollide_checkItem + 0x264
PATCH_B_END(KartCollide_checkItem, 0x254)

PATCH_REPLACE_START(KartCollide_checkItem, 0x264)
    bne 0x38c
PATCH_REPLACE_END(KartCollide_checkItem, 0x264)
