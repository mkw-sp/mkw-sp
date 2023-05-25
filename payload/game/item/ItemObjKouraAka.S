#include <Common.S>

PATCH_B_START(ItemObjKouraAka_vf_20, 0x128)
    mr r3, r29 // Original instruction

    lis r5, g_speedModFactor@ha
    lfs f2, g_speedModFactor@l (r5)
    fmuls f1, f1, f2
    b ItemObjKouraAka_vf_20 + 0x12c
PATCH_B_END(ItemObjKouraAka_vf_20, 0x128)

PATCH_B_START(ItemObjKouraAka_vf_20, 0x158)
    mr r3, r29 // Original instruction

    lis r5, g_speedModFactor@ha
    lfs f2, g_speedModFactor@l (r5)
    fmuls f1, f1, f2
    b ItemObjKouraAka_vf_20 + 0x15c
PATCH_B_END(ItemObjKouraAka_vf_20, 0x158)

PATCH_B_START(ItemObjKouraAka_807aae78, 0x9c)
    mr r3, r31 // Original instruction

    lis r5, g_speedModFactor@ha
    lfs f3, g_speedModFactor@l (r5)
    fmuls f1, f1, f3
    b ItemObjKouraAka_807aae78 + 0xa0
PATCH_B_END(ItemObjKouraAka_807aae78, 0x9c)
