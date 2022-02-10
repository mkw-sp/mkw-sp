#include <Common.S>

PATCH_REPLACE_START(Camera_805a1d10, 0x164)
    nop
PATCH_REPLACE_END(Camera_805a1d10, 0x164)

PATCH_BL_START(Camera_805a2034, 0x68)
    lwz r0, 0xb74 (r3) // Original instruction

    // Check that the current section is ghost watch
    lis r4, s_sectionManager@ha
    lwz r4, s_sectionManager@l (r4)
    lwz r4, 0x0 (r4)
    lwz r4, 0x0 (r4)
    cmpwi r4, 0x34
    bnelr

    // If that's the case pretend that it's live view to attach the camera to every player
    li r0, 0x6

    b Camera_805a2034 + 0x6c
PATCH_BL_END(Camera_805a2034, 0x68)

PATCH_B_START(Camera_805a2034, 0xbc)
    mr r29, r3
    lis r3, s_saveManager@ha
    lwz r3, s_saveManager@l (r3)
    bl SaveManager_getSetting169Fov
    slwi r3, r3, 4
    subf r3, r3, r29
    addi r0, r3, 0x10

    b Camera_805a2034 + 0xc0
PATCH_B_END(Camera_805a2034, 0xbc)

PATCH_BL_START(Camera_805a21d0, 0xf4)
    lhz r0, 0x334 (r29) // Original instruction

    // Check that the current section is ghost watch
    lis r4, s_sectionManager@ha
    lwz r4, s_sectionManager@l (r4)
    lwz r4, 0x0 (r4)
    lwz r4, 0x0 (r4)
    cmpwi r4, 0x34
    bnelr

    lwz r4, 0x340 (r29)
    cmpwi r4, 0x0
    beqlr

    b Camera_805a21d0 + 0xc4
PATCH_BL_END(Camera_805a21d0, 0xf4)

PATCH_BL_START(Camera_805a21d0, 0x7a0)
    // Check that the current section is not ghost watch
    lis r4, s_sectionManager@ha
    lwz r4, s_sectionManager@l (r4)
    lwz r4, 0x0 (r4)
    lwz r4, 0x0 (r4)
    cmpwi r4, 0x34
    beqlr

    ori r0, r0, 0x4 // Original instruction
    b Camera_805a21d0 + 0x7a4
PATCH_BL_END(Camera_805a21d0, 0x7a0)
