#include <Common.S>

PATCH_REPLACE_START(Camera_805a1d10, 0x164)
    nop
PATCH_REPLACE_END(Camera_805a1d10, 0x164)

PATCH_INJECT_START(Camera_805a2034, 0xbc)
    mr r29, r3
    lis r3, s_saveManager@ha
    lwz r3, s_saveManager@l (r3)
    bl SaveManager_getSetting169Fov
    slwi r3, r3, 4
    subf r3, r3, r29
    addi r0, r3, 0x10
PATCH_INJECT_END(Camera_805a2034, 0xbc)

.global fix_Camera_805a21d0_rearView
fix_Camera_805a21d0_rearView:
    lwz r0, 0x334 (r29) # Original instruction

    # Check if the current section is ghost replay
    lis r4, s_sectionManager@ha
    lwz r4, s_sectionManager@l (r4)
    lwz r4, 0x0 (r4)
    lwz r4, 0x0 (r4)
    cmpwi r4, 0x34
    bnelr

    lwz r4, 0x340 (r29)
    cmpwi r4, 0x0
    beqlr

    mflr r4
    subi r4, r4, 0x34
    mtlr r4
    blr
.size fix_Camera_805a21d0_rearView, . - fix_Camera_805a21d0_rearView

PATCH_BL(Camera_805a21d0 + 0xf4, fix_Camera_805a21d0_rearView)

PATCH_INJECT_START(Camera_805a21d0, 0x7a0)
    # Check if the current section is not ghost replay
    lis r4, s_sectionManager@ha
    lwz r4, s_sectionManager@l (r4)
    lwz r4, 0x0 (r4)
    lwz r4, 0x0 (r4)
    cmpwi r4, 0x34
    beq .Lend

    ori r0, r0, 0x4 # Original instruction

.Lend:
PATCH_INJECT_END(Camera_805a21d0, 0x7a0)
