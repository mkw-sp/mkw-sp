#include <Common.S>

# Fix the check for local Miis needed by the Mii randomization feature.
.global thunk_SaveManager_hasSpLicenseWithMiiId
thunk_SaveManager_hasSpLicenseWithMiiId:
    mflr r31

    lis r3, s_saveManager@ha
    lwz r3, s_saveManager@l (r3)
    addi r4, r30, 0x94
    bl SaveManager_hasSpLicenseWithMiiId

    mr r5, r3

    addi r31, r31, 0x114
    mtlr r31
    blr
.size thunk_SaveManager_hasSpLicenseWithMiiId, . - thunk_SaveManager_hasSpLicenseWithMiiId
PATCH_BL(Mii_80526020 + 0x1a4, thunk_SaveManager_hasSpLicenseWithMiiId)
