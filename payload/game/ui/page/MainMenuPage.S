#include <Common.S>

.global thunk_MainMenuPage_initMiiGroup
thunk_MainMenuPage_initMiiGroup:
    mflr r30

    bl MainMenuPage_initMiiGroup

    addi r30, r30, 0x7c
    mtlr r30
    blr
.size thunk_MainMenuPage_initMiiGroup, . - thunk_MainMenuPage_initMiiGroup
PATCH_BL(MainMenuPage_onInit + 0x20, thunk_MainMenuPage_initMiiGroup)

.global thunk_MainMenuPage_refreshFileAdminButton
thunk_MainMenuPage_refreshFileAdminButton:
    mflr r3
    stw r3, 0x8 (r1)

    mr r3, r30
    bl MainMenuPage_refreshFileAdminButton

    lwz r3, 0x8 (r1)
    addi r3, r3, 0x24
    mtlr r3
    blr
.size thunk_MainMenuPage_refreshFileAdminButton, . - thunk_MainMenuPage_refreshFileAdminButton
PATCH_BL(MainMenuPage_onActivate + 0x1dc, thunk_MainMenuPage_refreshFileAdminButton)
