#include <Common.S>

// Disable Mii randomization
PATCH_REPLACE_START(Mii_80526020, 0x170)
    b 0x214
PATCH_REPLACE_END(Mii_80526020, 0x170)
