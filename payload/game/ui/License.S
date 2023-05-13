#include <Common.S>

PATCH_B_START(LicenseDeletePage_onRefocus, 0x194)
    mr r3, r4
    bl SaveManager_EraseLicense // Original instruction

    bl SaveManager_EraseSPLicense

    b LicenseDeletePage_onRefocus + 0x198
PATCH_B_END(LicenseDeletePage_onRefocus, 0x194)



// Update the message id for settings

PATCH_REPLACE_START(LicenseManagementPage_onInit, 0x3f0)
    li r4, 10077
PATCH_REPLACE_END(LicenseManagementPage_onInit, 0x3f0)

PATCH_REPLACE_START(LicenseManagementPage_onActivate, 0x38)
    li r4, 10077
PATCH_REPLACE_END(LicenseManagementPage_onActivate, 0x38)

PATCH_REPLACE_START(LicenseManagementPage_onButtonSelect, 0x28)
    li r4, 10077
PATCH_REPLACE_END(LicenseManagementPage_onButtonSelect, 0x28)
