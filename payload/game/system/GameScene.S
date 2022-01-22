#include <Common.S>

PATCH_B_START(GameScene_8051a4dc, 0x440)
    lwz r3, 0xc94 (r31)
    lwz r4, 0xc98 (r31)
    bl DynMem_init

    lwz r3, 0xc94 (r31) // Original instruction
    b GameScene_8051a4dc + 0x444
PATCH_B_END(GameScene_8051a4dc, 0x440)

PATCH_B_START(GameScene_reinit, 0x20)
    bl DynMem_deinit

    mr r30, r29 // Original instruction
    b GameScene_reinit + 0x24
PATCH_B_END(GameScene_reinit, 0x20)
