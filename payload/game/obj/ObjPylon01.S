#include <Common.S>

// In time trials, instanciate one cone per player

PATCH_REPLACE_START(ObjPylon01_ct, 0x114)
    lis r3, s_raceConfig@ha
    lwz r3, s_raceConfig@l (r3)
    lbz r3, 0x20 + 0x4 (r3)
    addi r31, r31, 1
    cmpw r31, r3
    bge 0x44

    // Due to several limits in the game, we can only clone cones for the 4 first players
    cmpwi r31, 4
    bge 0x3c

    b 0x8
PATCH_REPLACE_END(ObjPylon01_ct, 0x114)

PATCH_REPLACE_START(ObjPylon01_ct, 0x150)
    mr r5, r31
PATCH_REPLACE_END(ObjPylon01_ct, 0x150)



// In time trials, check the player id

PATCH_REPLACE_START(ObjPylon01_vf_c0, 0x54)
    lwz r4, 0x108 (r27)
    cmpw r3, r4
    li r3, 0x0
    bne 0x36c

    b 0x40
PATCH_REPLACE_END(ObjPylon01_vf_c0, 0x54)
