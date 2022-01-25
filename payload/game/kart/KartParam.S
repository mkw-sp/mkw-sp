#include <Common.S>

PATCH_B_START(KartParam_compute, 0x1f0)
    lis r5, speedModFactor@ha
    lfs f2, speedModFactor@l (r5)

    lfs f3, 0xc (r3)
    fmuls f3, f3, f2
    stfs f3, 0xc (r3)

    fmuls f0, f0, f2 // Player base speed
    fmuls f1, f1, f2 // Vehicle base speed
    fadds f0, f1, f0 // Original instruction
    b KartParam_compute + 0x1f4
PATCH_B_END(KartParam_compute, 0x1f0)

PATCH_REPLACE_START(KartParam_compute, 0x210)
    fmadds f0, f1, f2, f0
PATCH_REPLACE_END(KartParam_compute, 0x210)

PATCH_REPLACE_START(KartParam_compute, 0x220)
    fmadds f0, f1, f2, f0
PATCH_REPLACE_END(KartParam_compute, 0x220)

PATCH_REPLACE_START(KartParam_compute, 0x230)
    fmadds f0, f1, f2, f0
PATCH_REPLACE_END(KartParam_compute, 0x230)

PATCH_REPLACE_START(KartParam_compute, 0x240)
    fmadds f0, f1, f2, f0
PATCH_REPLACE_END(KartParam_compute, 0x240)
