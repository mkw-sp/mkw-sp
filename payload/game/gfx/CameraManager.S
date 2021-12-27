#include <Common.S>

.global fix_CameraManager_ct
fix_CameraManager_ct:
    lis r3, s_sectionManager@ha
    lwz r3, s_sectionManager@l (r3)
    lwz r3, 0xc (r3)
    cmpwi r3, 0x34
    bne .Lend

    li r0, 0x1 # Set live view boolean to true if the next section is ghost replay

.Lend:
    stb r0, 0x25 (r31) # Original instruction, store live view boolean
.size fix_CameraManager_ct, . - fix_CameraManager_ct

PATCH_BL(CameraManager_ct + 0x70, fix_CameraManager_ct)
