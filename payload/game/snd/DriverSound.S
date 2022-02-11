#include <Common.S>

PATCH_BL_START(DriverSound_init, 0x68)
    // Unconditionally pretend that the player is not a ghost (at this point)
    li r4, 0x0
    stb r4, 0x6fc (r30)

    // If the player is really not a ghost, we are done
    cmpwi r0, 0x0
    beqlr

    // Otherwise pretend that the player is local
    li r4, 0x1
    stb r4, 0x6fa (r30)

    b DriverSound_init + 0x6c
PATCH_BL_END(DriverSound_init, 0x68)

PATCH_BL_START(DriverSound_init, 0xcc)
    clrlwi r0, r3, 24 // Original instruction

    // Check that the player is a ghost
    lwz r4, 0x6c0 (r30)
    lwz r4, 0x0 (r4)
    lwz r4, 0x4 (r4)
    lwz r4, 0x14 (r4)
    extrwi. r4, r4, 1, 25
    beqlr

    li r3, 0x0 // Set the local player id to 0

    clrlwi r0, r3, 24 // Original instruction again
    b DriverSound_init + 0xd0
PATCH_BL_END(DriverSound_init, 0xcc)
