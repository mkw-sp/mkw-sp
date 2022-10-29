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

PATCH_B_START(CameraManager_805a8fbc, 0x24)
    // Add start camera nullptr check

    cmpwi r4, 0x0
    beq .Lnull

    lhz r0, 0x6c (r4) // Original instruction
    b CameraManager_805a8fbc + 0x28

.Lnull:
    addi r6, r6, 0x1
    b CameraManager_805a8fbc + 0x34
PATCH_B_END(CameraManager_805a8fbc, 0x24)
