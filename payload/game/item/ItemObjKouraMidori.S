#include <Common.S>

PATCH_B_START(ItemObjKouraMidori_807aec64, 0x6c)
    lis r4, g_speedModFactor@ha
    lfs f2, g_speedModFactor@l (r4)
    fmuls f1, f1, f2

    bl Item_807b54f4 // Original instruction
    b ItemObjKouraMidori_807aec64 + 0x70
PATCH_B_END(ItemObjKouraMidori_807aec64, 0x6c)
