#include <Common.S>

PATCH_REPLACE_START(MenuScene_vf_38, 0xcc)
    // Do not load Earth.szs
    b 0x30
PATCH_REPLACE_END(MenuScene_vf_38, 0xcc)
