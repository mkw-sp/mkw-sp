#include <Common.S>

PATCH_REPLACE_START(SaveManagerProxy_init, 0x38)
    nop
PATCH_REPLACE_END(SaveManagerProxy_init, 0x38)

PATCH_REPLACE_START(SaveManagerProxy_processRequest, 0x74)
    nop
PATCH_REPLACE_END(SaveManagerProxy_processRequest, 0x74)
