#include <Common.S>

.set address, 0x80001800
.set magic, 0xD01F1BAD + 5

PATCH_BL_START(CtrlMenuPressStart_load, 0xa8)
    // Check that the Dolphin codehandler is enabled
    lis r3, address@ha
    lwz r3, address@l (r3)
    lis r4, magic@h
    ori r4, r4, magic@l
    cmpw r3, r4
    bnelr

    // Loudly tell the user about it
    lwz r3, 0x38 (r1)
    li r4, 0x3031
    li r5, 0x0
    bl LayoutUIControl_setMessageAll

    b CtrlMenuPressStart_load + 0xac
PATCH_BL_END(CtrlMenuPressStart_load, 0xa8)
