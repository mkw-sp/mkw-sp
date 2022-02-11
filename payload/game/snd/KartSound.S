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

PATCH_BL_START(KartSound_8070b250, 0x4c)
    lbz r0, 0xe0 (r29) // Original instruction

    // Check that the player is a ghost
    lwz r4, 0xdc (r29)
    lwz r4, 0x0 (r4)
    lwz r4, 0x4 (r4)
    lwz r4, 0x14 (r4)
    extrwi. r4, r4, 1, 25
    beqlr

    // If the player is supposedly local, pretend that's not the case
    cmpwi r0, 0x0
    li r0, 0x0
    bnelr

    // Otherwise do not try to play the lap jingle at all
    lwz r4, 0xc (r5)
    lwzx r3, r4, r3
    lwz r0, 0x38 (r3)
    extrwi r30, r0, 1, 30
    b KartSound_8070b250 + 0x21c
PATCH_BL_END(KartSound_8070b250, 0x4c)

PATCH_REPLACE_START(KartSound_80708b44, 0x28)
    blr // Remove section id check
PATCH_REPLACE_END(KartSound_80708b44, 0x28)
