#include <Common.S>

// Extend the Lakitu array to 12

PATCH_REPLACE_START(JugemManager_createInstance, 0x20)
    li r3, 0x14 + 12 * 0x4 + 0x4
PATCH_REPLACE_END(JugemManager_createInstance, 0x20)

PATCH_B_START(JugemManager_createInstance, 0x54)
    addi r3, r31, 0x14
    li r4, 0x0
    li r5, 12 * 0x4 + 0x4
    bl memset

    b JugemManager_createInstance + 0x68
PATCH_B_END(JugemManager_createInstance, 0x54)

PATCH_REPLACE_START(JugemManager_createJugems, 0xd8)
    lwz r0, 0x44 (r23)
PATCH_REPLACE_END(JugemManager_createJugems, 0xd8)

PATCH_REPLACE_START(JugemManager_createJugems, 0xec)
    lwz r0, 0x44 (r23)
PATCH_REPLACE_END(JugemManager_createJugems, 0xec)

PATCH_REPLACE_START(JugemManager_createJugems, 0x100)
    lwz r0, 0x44 (r23)
PATCH_REPLACE_END(JugemManager_createJugems, 0x100)

PATCH_REPLACE_START(JugemManager_createJugems, 0x11c)
    lwz r0, 0x44 (r23)
PATCH_REPLACE_END(JugemManager_createJugems, 0x11c)

PATCH_REPLACE_START(JugemManager_createJugems, 0x13c)
    lwz r0, 0x44 (r23)
PATCH_REPLACE_END(JugemManager_createJugems, 0x13c)

PATCH_REPLACE_START(JugemManager_createJugems, 0x15c)
    lwz r0, 0x44 (r23)
PATCH_REPLACE_END(JugemManager_createJugems, 0x15c)

PATCH_REPLACE_START(JugemManager_createJugems, 0x174)
    lwz r0, 0x44 (r23)
PATCH_REPLACE_END(JugemManager_createJugems, 0x174)

PATCH_REPLACE_START(JugemManager_createJugems, 0x188)
    lwz r3, 0x44 (r23)
PATCH_REPLACE_END(JugemManager_createJugems, 0x188)

PATCH_REPLACE_START(JugemManager_createJugems, 0x190)
    stw r0, 0x44 (r23)
PATCH_REPLACE_END(JugemManager_createJugems, 0x190)

PATCH_REPLACE_START(JugemManager_init, 0x68)
    cmpwi r31, 12
PATCH_REPLACE_END(JugemManager_init, 0x68)

PATCH_REPLACE_START(JugemManager_8071e734, 0x58)
    lwz r0, 0x44 (r27)
PATCH_REPLACE_END(JugemManager_8071e734, 0x58)

PATCH_REPLACE_START(JugemManager_8071e82c, 0x58)
    lwz r0, 0x44 (r27)
PATCH_REPLACE_END(JugemManager_8071e82c, 0x58)

PATCH_REPLACE_START(JugemManager_8071e8a4, 0x40)
    lwz r0, 0x44 (r29)
PATCH_REPLACE_END(JugemManager_8071e8a4, 0x40)

PATCH_REPLACE_START(JugemManager_8071e90c, 0x4c)
    lwz r0, 0x44 (r28)
PATCH_REPLACE_END(JugemManager_8071e90c, 0x4c)



// In ghost replay mode, create one Lakitu per player

PATCH_BL_START(JugemManager_createJugems, 0x1c)
    lbz r27, 0x20 + 0x5 (r3) // Original instruction

    // Check that the current section is ghost replay
    lis r4, s_sectionManager@ha
    lwz r4, s_sectionManager@l (r4)
    lwz r4, 0x0 (r4)
    lwz r4, 0x0 (r4)
    cmpwi r4, 0x34
    bnelr

    // Create one Lakitu per player
    lbz r27, 0x20 + 0x4 (r3)

    b JugemManager_createJugems + 0x20
PATCH_BL_END(JugemManager_createJugems, 0x1c)

PATCH_BL_START(JugemManager_createJugems, 0x84)
    lwz r3, s_raceConfig@l (r30) // Original instruction

    // Check that the current section is ghost replay
    lis r4, s_sectionManager@ha
    lwz r4, s_sectionManager@l (r4)
    lwz r4, 0x0 (r4)
    lwz r4, 0x0 (r4)
    cmpwi r4, 0x34
    bnelr

    mr r4, r24

    b JugemManager_createJugems + 0x94
PATCH_BL_END(JugemManager_createJugems, 0x84)

PATCH_BL_START(JugemManager_createJugems, 0xdc)
    clrlwi r4, r24, 24

    // Check that the current section is ghost replay
    lis r5, s_sectionManager@ha
    lwz r5, s_sectionManager@l (r5)
    lwz r5, 0x0 (r5)
    lwz r5, 0x0 (r5)
    cmpwi r5, 0x34
    bnelr

    li r4, 0x0

    b JugemManager_createJugems + 0xe0
PATCH_BL_END(JugemManager_createJugems, 0xdc)
