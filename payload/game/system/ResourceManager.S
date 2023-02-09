#include <Common.S>

PATCH_REPLACE_START(ResourceManager_attachLayoutDir, 0x40)
    // Use the correct menu archive count to exit the loop
    // Branches to ResourceManager_attachLayoutDir + 0x10c
    bge 0xcc
PATCH_REPLACE_END(ResourceManager_attachLayoutDir, 0x40)

PATCH_B_START(ResourceManager_attachLayoutDir, 0x8c)
    mr r3, r27
    mr r4, r31
    bl _ZN6System15ResourceManager14getMenuArchiveEj
    lwz r4, 0x8 (r3)

    mr r3, r26 // Original instruction
    b ResourceManager_attachLayoutDir + 0xac
PATCH_B_END(ResourceManager_attachLayoutDir, 0x8c)
