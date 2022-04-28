#include <Common.S>

PATCH_BL_START(nw4r_db_ShowStack_, 0x6C)
    b StackTrace_TryDecryptLinkRegister
PATCH_BL_END(nw4r_db_ShowStack_, 0x6C)
