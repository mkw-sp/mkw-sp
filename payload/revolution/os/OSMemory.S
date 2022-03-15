#include <Common.S>

# Invalidate IBAT4
PATCH_REPLACE_START(ConfigMEM2_56MB, 0x3C)
    mtspr 561, r7 # IBAT4L
    nop
PATCH_REPLACE_END(ConfigMEM2_56MB, 0x3C)

# Invalidate IBAT6
PATCH_REPLACE_START(ConfigMEM2_56MB, 0x9C)
    mtspr 565, r7 # IBAT6L
    nop
PATCH_REPLACE_END(ConfigMEM2_56MB, 0x9C)

# Invalidate IBAT7
PATCH_REPLACE_START(ConfigMEM2_56MB, 0xBC)
    mtspr 567, r7 # IBAT7L
    nop
PATCH_REPLACE_END(ConfigMEM2_56MB, 0xBC)
