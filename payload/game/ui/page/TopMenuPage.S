#include <Common.S>

.global thunk_TopMenuPage_initMiiGroup
thunk_TopMenuPage_initMiiGroup:
    mflr r30

    bl TopMenuPage_initMiiGroup

    addi r30, r30, 0x7c
    mtlr r30
    blr
.size thunk_TopMenuPage_initMiiGroup, . - thunk_TopMenuPage_initMiiGroup
PATCH_BL(TopMenuPage_onInit + 0x20, thunk_TopMenuPage_initMiiGroup)

.global thunk_TopMenuPage_refreshFileAdminButton
thunk_TopMenuPage_refreshFileAdminButton:
    mflr r3
    stw r3, 0x8 (r1)

    mr r3, r30
    bl TopMenuPage_refreshFileAdminButton

    lwz r3, 0x8 (r1)
    addi r3, r3, 0x24
    mtlr r3
    blr
.size thunk_TopMenuPage_refreshFileAdminButton, . - thunk_TopMenuPage_refreshFileAdminButton
PATCH_BL(TopMenuPage_onActivate + 0x1dc, thunk_TopMenuPage_refreshFileAdminButton)
