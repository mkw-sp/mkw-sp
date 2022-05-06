#include <Common.S>

PATCH_B_START(ResourceManager_attachLayoutDir, 0x8c)
    mr r3, r27
    mr r4, r31
    bl ResourceManager_getMenuArchive
    lwz r4, 0x8 (r3)

    mr r3, r26 // Original instruction
    b ResourceManager_attachLayoutDir + 0xac
PATCH_B_END(ResourceManager_attachLayoutDir, 0x8c)
