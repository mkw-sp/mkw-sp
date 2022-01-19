#include <Common.S>

PATCH_INJECT_START(RawLicense_reset, 0x64)
    // Unlock everything
    lis r9, 0xffff
    ori r9, r9, 0xffff
    stw r9, 0x30 (r31)
    lis r9, 0x031f
    ori r9, r9, 0xfffc
    stw r9, 0x34 (r31)
PATCH_INJECT_END(RawLicense_reset, 0x64)
