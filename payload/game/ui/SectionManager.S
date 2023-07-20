#include <Common.S>

// Essentially a giant hack.
// Decompiling SectionManager::init to support resizing the allocation of GlobalContext would be a
// lot more work than this. At some point, that should happen and this file should stop existing.

PATCH_B_START(SectionManager_init, 0x8c)
        lis r3, GlobalContext_SizeInBytes@ha
        lha r3, GlobalContext_SizeInBytes@l(r3)
        b SectionManager_init + 0x90
PATCH_B_END(SectionManager_init, 0x8c)
