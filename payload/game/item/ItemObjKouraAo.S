#include <Common.S>

PATCH_B_START(ItemObjKouraAo_807ac49c, 0xcc)
    mr r3, r30 // Original instruction

    lis r4, speedModFactor@ha
    lfs f7, speedModFactor@l (r4)
    fmuls f3, f3, f7
    b ItemObjKouraAo_807ac49c + 0xd0
PATCH_B_END(ItemObjKouraAo_807ac49c, 0xcc)

PATCH_B_START(ItemObjKouraAo_807ad5a4, 0x18)
    mr r31, r3 // Original instruction

    lis r4, speedModFactor@ha
    lfs f2, speedModFactor@l (r4)
    fmuls f1, f1, f2
    b ItemObjKouraAo_807ad5a4 + 0x1c
PATCH_B_END(ItemObjKouraAo_807ad5a4, 0x18)

PATCH_B_START(ItemObjKouraAo_807ad824, 0x24)
    lfs f2, 0x2f0 (r31) // Original instruction

    lis r4, speedModFactor@ha
    lfs f4, speedModFactor@l (r4)
    fmuls f3, f3, f4
    b ItemObjKouraAo_807ad824 + 0x28
PATCH_B_END(ItemObjKouraAo_807ad824, 0x24)

PATCH_B_START(ItemObjKouraAo_807ad824, 0x8c)
    fmuls f1, f1, f1 // Original instruction

    lis r4, speedModFactor@ha
    lfs f4, speedModFactor@l (r4)
    fmuls f0, f0, f4
    b ItemObjKouraAo_807ad824 + 0x90
PATCH_B_END(ItemObjKouraAo_807ad824, 0x8c)

PATCH_B_START(ItemObjKouraAo_807ad958, 0x24)
    lfs f1, 0x2f0 (r3) // Original instruction

    lis r4, speedModFactor@ha
    lfs f3, speedModFactor@l (r4)
    fmuls f2, f2, f3
    b ItemObjKouraAo_807ad958 + 0x28
PATCH_B_END(ItemObjKouraAo_807ad958, 0x24)
