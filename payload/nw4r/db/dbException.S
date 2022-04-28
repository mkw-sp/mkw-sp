#include <Common.S>

// Display all exception information when an exception occurs
PATCH_REPLACE_START(nw4r_db_Exception_Init, 0x34)
    li r0, 0x3F
PATCH_REPLACE_END(nw4r_db_Exception_Init, 0x34)

PATCH_BL_START(Exception_PrintCtx, 0x2AC)
    b StackTrace_TryDecryptLinkRegister
PATCH_BL_END(Exception_PrintCtx, 0x2AC)
