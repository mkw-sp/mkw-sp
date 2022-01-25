#include <Common.S>

PATCH_BL_START(CameraManager_ct, 0x70)
    stb r0, 0x25 (r31) // Original instruction, store live view boolean

    lis r3, s_sectionManager@ha
    lwz r3, s_sectionManager@l (r3)
    lwz r3, 0xc (r3)
    cmpwi r3, 0x34
    bnelr

    li r0, 0x1 // Set live view boolean to true if the next section is ghost replay
    stb r0, 0x25 (r31)

    b CameraManager_ct + 0x74
PATCH_BL_END(CameraManager_ct, 0x70)
