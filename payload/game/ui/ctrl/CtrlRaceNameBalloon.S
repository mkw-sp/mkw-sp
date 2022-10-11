#include <Common.S>

// Increase the size of nameIsEnabled from 3 to 12

PATCH_REPLACE_START(CtrlRaceNameBalloon_initSelf, 0x144)
    cmpwi r4, 12
PATCH_REPLACE_END(CtrlRaceNameBalloon_initSelf, 0x144)

PATCH_REPLACE_START(CtrlRaceNameBalloon_calcSelf, 0x1e4)
    cmpwi r4, 12
PATCH_REPLACE_END(CtrlRaceNameBalloon_calcSelf, 0x1e4)

PATCH_REPLACE_START(CtrlRaceNameBalloon_calcSelf, 0x408)
    cmpwi r4, 12
PATCH_REPLACE_END(CtrlRaceNameBalloon_calcSelf, 0x408)

PATCH_REPLACE_START(CtrlRaceNameBalloon_calcPosition, 0x264)
    cmpwi r4, 12
PATCH_REPLACE_END(CtrlRaceNameBalloon_calcPosition, 0x264)

// Move nameIsEnabled from 0x0 to 0x44

PATCH_B_START(CtrlRaceNameBalloon_initSelf, 0x15c)
    add r3, r3, r4
    stb r0, 0x44 (r3)

    b CtrlRaceNameBalloon_initSelf + 0x160
PATCH_B_END(CtrlRaceNameBalloon_initSelf, 0x15c)

PATCH_B_START(CtrlRaceNameBalloon_calcSelf, 0x1fc)
    add r3, r3, r4
    stb r0, 0x44 (r3)

    b CtrlRaceNameBalloon_calcSelf + 0x200
PATCH_B_END(CtrlRaceNameBalloon_calcSelf, 0x1fc)

PATCH_B_START(CtrlRaceNameBalloon_calcSelf, 0x420)
    add r3, r3, r4
    stb r0, 0x44 (r3)

    b CtrlRaceNameBalloon_calcSelf + 0x424
PATCH_B_END(CtrlRaceNameBalloon_calcSelf, 0x420)

PATCH_REPLACE_START(CtrlRaceNameBalloon_calcSelf, 0x50c)
    lbz r0, 0x44 (r3)
PATCH_REPLACE_END(CtrlRaceNameBalloon_calcSelf, 0x50c)

PATCH_B_START(CtrlRaceNameBalloon_calcSelf, 0x52c)
    add r4, r6, r4
    stb r3, 0x44 (r4)

    b CtrlRaceNameBalloon_calcSelf + 0x530
PATCH_B_END(CtrlRaceNameBalloon_calcSelf, 0x52c)

PATCH_B_START(CtrlRaceNameBalloon_calcPosition, 0x27c)
    add r3, r3, r4
    stb r0, 0x44 (r3)

    b CtrlRaceNameBalloon_calcPosition + 0x280
PATCH_B_END(CtrlRaceNameBalloon_calcPosition, 0x27c)

PATCH_REPLACE_START(BalloonManager_calc, 0x94)
    stb r4, 0x44 (r3)
PATCH_REPLACE_END(BalloonManager_calc, 0x94)

PATCH_REPLACE_START(BalloonManager_calc, 0xe8)
    stb r5, 0x44 (r3)
PATCH_REPLACE_END(BalloonManager_calc, 0xe8)

PATCH_REPLACE_START(BalloonManager_calc, 0x168)
    stb r5, 0x44 (r3)
PATCH_REPLACE_END(BalloonManager_calc, 0x168)

PATCH_B_START(BalloonManager_calc, 0x4cc)
    add r3, r30, r15
    stb r26, 0x44 (r3)

    b BalloonManager_calc + 0x4d0
PATCH_B_END(BalloonManager_calc, 0x4cc)

PATCH_B_START(BalloonManager_calc, 0x734)
    add r3, r30, r4
    stb r26, 0x44 (r3)

    b BalloonManager_calc + 0x738
PATCH_B_END(BalloonManager_calc, 0x734)

PATCH_B_START(BalloonManager_calc, 0x810)
    add r3, r30, r5
    stb r26, 0x44 (r3)

    b BalloonManager_calc + 0x814
PATCH_B_END(BalloonManager_calc, 0x810)

// Move namePositions to 0x50 and add a level of indirection

PATCH_B_START(CtrlRaceNameBalloon_calcPosition, 0x94)
    lwz r6, 0x50 (r4)
    add r6, r6, r0
    lwz r5, 0x0 (r6)
    lwz r4, 0x4 (r6)
    lwz r0, 0x8 (r6)
    b CtrlRaceNameBalloon_calcPosition + 0xa4
PATCH_B_END(CtrlRaceNameBalloon_calcPosition, 0x94)

PATCH_B_START(BalloonManager_calc, 0x42c)
    lwz r16, 0x50 (r30)
    add r16, r16, r0
    stfs f0, 0x0 (r16)
    lfs f0, 0x30 (r1)
    stfs f0, 0x4 (r16)
    lfs f0, 0x34 (r1)
    stfs f0, 0x8 (r16)
    b BalloonManager_calc + 0x444
PATCH_B_END(BalloonManager_calc, 0x42c)

PATCH_REPLACE_START(BalloonManager_calc, 0x74c)
    stfs f0, 0x0 (r16)
    lfs f0, 0x30 (r1)
    stfs f0, 0x4 (r16)
    lfs f0, 0x34 (r1)
    stfs f0, 0x8 (r16)
PATCH_REPLACE_END(BalloonManager_calc, 0x74c)

PATCH_REPLACE_START(BalloonManager_calc, 0x7b0)
    lfs f0, 0xcc (r4)
PATCH_REPLACE_END(BalloonManager_calc, 0x7b0)

// Initialize lastWatchedPlayerId too
PATCH_REPLACE_START(CtrlRaceNameBalloon_ct, 0x30)
    stw r4, 0x174 (r31)
PATCH_REPLACE_END(CtrlRaceNameBalloon_ct, 0x30)

PATCH_BL_START(BalloonManager_calc, 0x48)
    li r4, 0x2 // Original instruction

    // Check that the current section id is ghost replay
    lis r5, s_sectionManager@ha
    lwz r5, s_sectionManager@l (r5)
    lwz r5, 0x0 (r5)
    lwz r5, 0x0 (r5)
    cmpwi r5, 0x34
    bnelr

    mflr r25
    mr r24, r3

    // Check that the tag visibility rule is "watched"
    bl SaveManager_GetTAGhostTagVisibility
    cmpwi r3, 0x1
    mr r3, r24
    mtlr r25
    bnelr

    // Always try to display tags for every player. The actual visibility logic is moved to the tags
    // themselves, allowing for faster switching.
    b BalloonManager_calc + 0x18c
PATCH_BL_END(BalloonManager_calc, 0x48)

PATCH_B_START(CtrlRaceNameBalloon_calcSelf, 0x4e8)
    bl CtrlRaceNameBalloon_calcPosition // Original instruction

    mr r3, r31
    bl _ZN2UI19CtrlRaceNameBalloon14calcVisibilityEv

    b CtrlRaceNameBalloon_calcSelf + 0x4ec
PATCH_B_END(CtrlRaceNameBalloon_calcSelf, 0x4e8)

PATCH_B_START(CtrlRaceNameBalloon_calcSelf, 0x620)
    bl CtrlRaceNameBalloon_calcPosition // Original instruction

    mr r3, r31
    bl _ZN2UI19CtrlRaceNameBalloon14calcVisibilityEv

    b CtrlRaceNameBalloon_calcSelf + 0x624
PATCH_B_END(CtrlRaceNameBalloon_calcSelf, 0x620)

PATCH_BL_START(BalloonManager_calc, 0x7c)
    stw r0, 0x14 (r30) // Original instruction

    // Check that the current section id is ghost replay
    lis r3, s_sectionManager@ha
    lwz r3, s_sectionManager@l (r3)
    lwz r3, 0x0 (r3)
    lwz r3, 0x0 (r3)
    cmpwi r3, 0x34
    bnelr

    // Check that there are at least two ghosts
    lis r3, s_raceConfig@ha
    lwz r3, s_raceConfig@l (r3)
    lbz r3, 0x24 (r3)
    cmpwi r3, 2
    bltlr

    // Hide the tagline at the beginning of the race
    li r0, -1
    stw r0, 0x14 (r30)

    b BalloonManager_calc + 0x80
PATCH_BL_END(BalloonManager_calc, 0x7c)

// Fix disappearing names in rear view
PATCH_REPLACE_START(BalloonManager_calc, 0x4ec)
    b 0x334
PATCH_REPLACE_END(BalloonManager_calc, 0x4ec)

// Wait a shorter time before displaying tags
PATCH_REPLACE_START(BalloonManager_calc, 0x128)
    li r0, 300
PATCH_REPLACE_END(BalloonManager_calc, 0x128)
