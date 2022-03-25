#include <Common.S>

# Invalidate IBAT4
PATCH_REPLACE_START(ConfigMEM2_56MB, 0x3C)
    mtspr IBAT4L, r7
    nop
PATCH_REPLACE_END(ConfigMEM2_56MB, 0x3C)

# Invalidate IBAT6
PATCH_REPLACE_START(ConfigMEM2_56MB, 0x9C)
    mtspr IBAT6L, r7
    nop
PATCH_REPLACE_END(ConfigMEM2_56MB, 0x9C)

# Invalidate IBAT7
PATCH_REPLACE_START(ConfigMEM2_56MB, 0xBC)
    mtspr IBAT7L, r7
    nop
PATCH_REPLACE_END(ConfigMEM2_56MB, 0xBC)
