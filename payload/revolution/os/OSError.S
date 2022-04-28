#include <Common.S>

PATCH_BL_START(OSPanic, 0xDC)
    b StackTrace_TryDecryptLinkRegister
PATCH_BL_END(OSPanic, 0xDC)
