#include <Common.S>

// Display all exception information when an exception occurs
PATCH_REPLACE_START(nw4r_db_Exception_Init, 0x34)
    li r0, 0x3F
PATCH_REPLACE_END(nw4r_db_Exception_Init, 0x34)

PATCH_BL_START(Exception_PrintCtx, 0x2A4)
    lwz       r6, 4(r29) # Original instruction
    cmpwi     r6, 0
    bltlr-

    lis       r7, __stack_chk_guard@ha
    lwz       r7, __stack_chk_guard@l(r7)
    xor       r6, r6, r7
    oris      r6, r6, 0x8000
    blr
PATCH_BL_END(Exception_PrintCtx, 0x2A4)
