#include <Common.S>

PATCH_B_START(LicenseDisplayPage_onActivate, 0xd8)
    stb r31, 0x4c0 (r3) // Original instruction

    lis r3, s_saveManager@ha
    lwz r3, s_saveManager@l (r3)
    addi r4, r27, 0x94
    bl SaveManager_createSpLicense

    b LicenseDisplayPage_onActivate + 0xdc
PATCH_B_END(LicenseDisplayPage_onActivate, 0xd8)

PATCH_B_START(LicenseDisplayPage_onActivate, 0x22c)
    bl MiiGroup_insertFromId // Original instruction

    lis r3, s_saveManager@ha
    lwz r3, s_saveManager@l (r3)
    addi r4, r31, 0x94
    bl SaveManager_changeSpLicenseMiiId

    b LicenseDisplayPage_onActivate + 0x230
PATCH_B_END(LicenseDisplayPage_onActivate, 0x22c)

PATCH_B_START(LicenseDeletePage_onRefocus, 0x194)
    bl SaveManager_eraseLicense // Original instruction

    lis r3, s_saveManager@ha
    lwz r3, s_saveManager@l (r3)
    bl SaveManager_eraseSpLicense

    b LicenseDeletePage_onRefocus + 0x198
PATCH_B_END(LicenseDeletePage_onRefocus, 0x194)
