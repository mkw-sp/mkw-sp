#include <Common.S>

PATCH_REPLACE_START(CtrlMenuCharacterSelect_loadDriverButton, 0x5C)
    // This will always take the next bne
    nop
PATCH_REPLACE_END(CtrlMenuCharacterSelect_loadDriverButton, 0x5C)

PATCH_B_START(CtrlMenuCharacterSelect_loadDriverButton, 0x98)
    lis     r5, CharacterSelectMultiCtrl_str@ha
    addi    r5, r5, CharacterSelectMultiCtrl_str@l
    b       CtrlMenuCharacterSelect_loadDriverButton + 0x9C
PATCH_B_END(CtrlMenuCharacterSelect_loadDriverButton, 0x98)

// Force buttons to not be player exclusive
PATCH_REPLACE_START(CtrlMenuCharacterSelect_loadDriverButton, 0xD4)
    li      r8, 0
PATCH_REPLACE_END(CtrlMenuCharacterSelect_loadDriverButton, 0xD4)

PATCH_B_START(CtrlMenuCharacterSelect_loadDriverButton, 0x110)
    // This bool is normally set for Mii buttons, but we repurpose the layout for the normal multiplayer buttons.
    // Set it to true if the local player count (r29) is not 1.
    xori    r5, r29, 1
    subic   r0, r5, 1
    subfe   r0, r0, r5
    stb     r0, 0x254(r4)
    b       CtrlMenuCharacterSelect_loadDriverButton + 0x114
PATCH_B_END(CtrlMenuCharacterSelect_loadDriverButton, 0x110)

PATCH_BL_START(CtrlMenuCharacterSelect_onFront, 0xA8)
    lbz     r0, 0x254(r3) // Is multiplayer button
    cmpwi   r0, 0
    bnelr
    b       LayoutUIControl_setMessageAll
PATCH_BL_END(CtrlMenuCharacterSelect_onFront, 0xA8)

// Change the ctrl name to what it actually is
    .section .rodata
CharacterSelectMultiCtrl_str:
    .string "CharacterSelect%d_%d_Multi"
