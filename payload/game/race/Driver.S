#include <Common.S>

PATCH_REPLACE_START(Driver_807cb198, 0x164)
    // Do not treat non-focused live view players differently
    b 0x30
PATCH_REPLACE_END(Driver_807cb198, 0x164)
