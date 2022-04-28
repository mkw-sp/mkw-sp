#include <Common.S>

PATCH_BL_START(_unresolved, 0x58)
    b StackTrace_TryDecryptLinkRegister
PATCH_BL_END(_unresolved, 0x58)
