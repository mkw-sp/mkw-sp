#include <Common.S>

PATCH_B_START(CtrlRace2DMap_load, 0x38)
    // Reset the unfocused icon and shadow scales
    lis r3, 0x3f19
    ori r3, r3, 0x999a
    stw r3, 0x18 (r31)
    lis r3, 0x3f26
    ori r3, r3, 0x6666
    stw r3, 0x1c (r31)

    cmpwi r0, 0x2 // Original instruction
    b CtrlRace2DMap_load + 0x3c
PATCH_B_END(CtrlRace2DMap_load, 0x38)

PATCH_BL_START(CtrlRace2DMap_load, 0x48)
    lfs f1, 0x10 (r31) // Original instruction

    mflr r26
    mr r25, r5
    bl SaveManager_GetVanillaMode
    mr r5, r25
    mtlr r26

    // If vanilla mode is enabled, change the unfocused icon and shadow scales.
    cmpwi r3, 0x0
    bnelr

    // Otherwise, skip it.
    b CtrlRace2DMap_load + 0x58
PATCH_BL_END(CtrlRace2DMap_load, 0x48)

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
PATCH_B_START(CtrlRace2DMapCharacter_calcTransform, 0x314)
    stfs f0, 0x48 (r3) // Original instruction

    lfs f3, 0x78 (r30)

    lfs f2, 0x7c (r28)
    fadds f2, f2, f3
    stfs f2, 0x7c (r28)

    lwz r3, 0x198 (r28)
    lfs f2, 0x34 (r3)
    fadds f2, f2, f3
    stfs f2, 0x34 (r3)

    b CtrlRace2DMapCharacter_calcTransform + 0x318
PATCH_B_END(CtrlRace2DMapCharacter_calcTransform, 0x314)

// Fix copy-pasting mistake
PATCH_REPLACE_START(CtrlRace2DMapCharacter_calcTransform, 0x5bc)
    lwz r3, 0x1c0 (r28)
PATCH_REPLACE_END(CtrlRace2DMapCharacter_calcTransform, 0x5bc)
