#include <Common.S>

PATCH_INJECT_START(TitlePage_onInit, 0x16c)
    # Enable alternative backgrounds
    li r0, 0x1
    stb r0, 0x348 (r31)
PATCH_INJECT_END(TitlePage_onInit, 0x16c)
