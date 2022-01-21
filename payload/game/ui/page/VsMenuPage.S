#include <Common.S>

PATCH_B_START(VsMenuPage_onActivate, 0x104)
    mr r3, r0
    bl VsMenuPage_applyClassRule

    b VsMenuPage_onActivate + 0x1b4
PATCH_B_END(VsMenuPage_onActivate, 0x104)
