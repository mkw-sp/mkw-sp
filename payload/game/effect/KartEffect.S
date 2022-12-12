#include <Common.S>

PATCH_REPLACE_START(KartEffect_construct, 0x530)
    cmpwi r0, 0x6
PATCH_REPLACE_END(KartEffect_construct, 0x530)
