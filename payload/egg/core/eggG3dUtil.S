#include <Common.S>

PATCH_B_START(g3dutil_SearchSamplers, 0x68)
    cmpwi r3, 0
    beq InvalidTexRef
    // e.g. 005bb01d
    srwi r12, r3, 31
    cmpwi r12, 0
    beq InvalidTexRef
        lwz r0, 0(r3) // Original instruction
        b g3dutil_SearchSamplers+0x6c
    InvalidTexRef:
        // Get name
        lwz r3, 0(r23)
        lwz r4, 0x48(r3)
        add r3, r3, r4
        bl InvalidTexRefFail
        // Unreachable
PATCH_B_END(g3dutil_SearchSamplers, 0x68)

// The same crash, but a bit further down because sampler (garbage pointer) -> name
// happens to be zero (chance).
PATCH_B_START(g3dutil_SearchSamplers, 0x7c)
    // This is already a failure bound path
    lwz r3, 0(r23)
    lwz r4, 0x48(r3)
    add r3, r3, r4
    bl InvalidTexRefFail
    // Unreachable
PATCH_B_END(g3dutil_SearchSamplers, 0x7c)
