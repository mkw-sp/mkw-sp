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
