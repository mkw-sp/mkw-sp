#include <Common.S>

PATCH_B_START(RawLicense_reset, 0x64)
    // Unlock everything
    lis r9, 0xffff
    ori r9, r9, 0xffff
    stw r9, 0x30 (r31)
    lis r9, 0x031f
    ori r9, r9, 0xfffc
    stw r9, 0x34 (r31)
    b RawLicense_reset + 0x68
PATCH_B_END(RawLicense_reset, 0x64)

PATCH_B_START(RawLicense_reset, 0x140)
    lis r3, s_saveManager@ha
    lwz r3, s_saveManager@l (r3)
    li r4, kSetting_DriftMode
    bl SaveManager_getSetting
    addi r3, r3, 0x1
    lhz r4, 0xea (r31)
    rlwimi r4, r3, 14, 16, 17
    sth r4, 0xea (r31)

    lwz r0, 0x34 (r1) // Original instruction
    b RawLicense_reset + 0x144
PATCH_B_END(RawLicense_reset, 0x140)
