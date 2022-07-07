#include <Common.S>

PATCH_REPLACE_START(CtrlRace2DMap_load, 0x44)
    // Always set the focused player icon scale to 0.8
    lfs f1, 0x10 (r31)
    stfs f1, 0x1c (r31)

    // Keep the non-focused one at 0.6
    b 0xc
PATCH_REPLACE_END(CtrlRace2DMap_load, 0x44)

PATCH_BL_START(CtrlRace2DMapCharacter_initSelf, 0x294)
    lwz r4, 0x20 + 0x8 + 0x10 (r3) // Get the player type

    lwz r3, 0x20 + 0x8 + 0xc (r3) // Get the character id

    // Check that the character id is not already a Mii
    cmpwi r3, 0x18
    bgelr

    // Check that the player type is not cpu
    cmpwi r4, 0x1
    beqlr

    // Check that we are not in split-screen mode
    lis r4, s_raceConfig@ha
    lwz r4, s_raceConfig@l (r4)
    lbz r4, 0x20 + 0x6 (r4)
    cmpwi r4, 0x1
    bgtlr

    mflr r29
    mr r28, r3

    bl SaveManager_GetMapIcons
    cmpwi r3, 0x1

    mr r3, r28
    mtlr r29

    // Check that the setting is enabled
    bnelr

    // Set the id to a Mii one (it's only used for the comparison)
    li r3, 0x18
    b CtrlRace2DMapCharacter_initSelf + 0x298
PATCH_BL_END(CtrlRace2DMapCharacter_initSelf, 0x294)

PATCH_B_START(CtrlRace2DMapCharacter_calcTransform, 0x138)
    // Check if the section id is ghost replay
    cmpwi r0, 0x34
    bne .Lend

    li r0, 0x6a // Pretend it's live view instead

.Lend:
    cmpwi r0, 0x6a // Original instruction

    b CtrlRace2DMapCharacter_calcTransform + 0x13c
PATCH_B_END(CtrlRace2DMapCharacter_calcTransform, 0x138)

// Make the icon of the selected player appear over the other ones
PATCH_B_START(CtrlRace2DMapCharacter_calcTransform, 0x298)
    stfs f0, 0x48 (r3) // Original instruction

    lfs f3, 0x78 (r30)

    lfs f2, 0x7c (r28)
    fadds f2, f2, f3
    stfs f2, 0x7c (r28)

    lwz r3, 0x198 (r28)
    lfs f2, 0x34 (r3)
    fadds f2, f2, f3
    stfs f2, 0x34 (r3)

    b CtrlRace2DMapCharacter_calcTransform + 0x29c
PATCH_B_END(CtrlRace2DMapCharacter_calcTransform, 0x298)

// Always hide shadows
PATCH_REPLACE_START(CtrlRace2DMapCharacter_calcTransform, 0x5b4)
    stb r0, 0xbb (r3)
    lwz r3, 0x1c0 (r28)
    lbz r0, 0xbb (r3)
    rlwinm r0, r0, 0, 18, 30
    stb r0, 0xbb (r3)
    b 0x2c
PATCH_REPLACE_END(CtrlRace2DMapCharacter_calcTransform, 0x5b4)
