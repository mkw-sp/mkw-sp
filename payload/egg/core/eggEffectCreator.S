#include <Common.S>

PATCH_B_START(EffectCreator_createEffect, 0x34)
    lwz r3, 8(r1) // Original instruction
    cmpwi r3, 0
    beq MissingBREFF
        b EffectCreator_createEffect+0x38
    MissingBREFF:
        mr r3, r30
        bl MissingBreffFail
        // Unreachable
PATCH_B_END(EffectCreator_createEffect, 0x34)
