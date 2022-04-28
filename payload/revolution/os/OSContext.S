#include <Common.S>

PATCH_BL_START(OSDumpContext, 0x21C)
    b StackTrace_TryDecryptLinkRegister
PATCH_BL_END(OSDumpContext, 0x21C)
