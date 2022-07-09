#include <Common.S>

PATCH_BL_START(KartSound_init, 0x6c)
    stb r3, 0xb3 (r31) // Original instruction

    // Check that the player is a ghost
    lbz r3, 0xe1 (r31)
    cmpwi r3, 0x0
    beqlr

    // Set the local player id to 0
    li r3, 0x0
    stb r3, 0xb3 (r31)

    b KartSound_init + 0x70
PATCH_BL_END(KartSound_init, 0x6c)

PATCH_BL_START(KartSound_init, 0xa4)
    clrlwi. r0, r3, 24 // Original instruction, check if the player is local
    bnelr // If that's the case we are done

    // Otherwise check if the player is a ghost
    lbz r3, 0xe1 (r31)

    // Pretend that the player is not a ghost (at this point)
    li r4, 0x0
    stb r4, 0xe1 (r31)

    clrlwi. r0, r3, 24 // Original instruction again
    b KartSound_init + 0xa8
PATCH_BL_END(KartSound_init, 0xa4)

PATCH_REPLACE_START(KartSound_80708b44, 0x28)
    blr // Remove section id check
PATCH_REPLACE_END(KartSound_80708b44, 0x28)
