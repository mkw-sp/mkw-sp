#include <Common.S>

// In stock, NANDManager is only used in SaveManager and other rewritten functions.

PATCH_REPLACE_START(NandManager_createInstance, 0x0)
    blr
PATCH_REPLACE_END(NandManager_createInstance, 0x0)

PATCH_REPLACE_START(NandManager_init, 0x0)
    blr
PATCH_REPLACE_END(NandManager_init, 0x0)
