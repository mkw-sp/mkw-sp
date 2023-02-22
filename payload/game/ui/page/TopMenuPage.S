#include <Common.S>

PATCH_B_START(TopMenuPage_onInit, 0x20)
    bl TopMenuPage_initMiiGroup

    b TopMenuPage_onInit + 0xa0
PATCH_B_END(TopMenuPage_onInit, 0x20)

PATCH_B_START(TopMenuPage_onActivate, 0x1dc)
    mr r3, r30
    bl TopMenuPage_refreshFileAdminButton

    b TopMenuPage_onActivate + 0x204
PATCH_B_END(TopMenuPage_onActivate, 0x1dc)
