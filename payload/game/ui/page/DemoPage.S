#include <Common.S>

// Hide both controls

PATCH_B_START(DemoPage_onInit, 0x12c)
    li r5, 0x1
    stb r5, 0x44 + 0x80 (r30)
    stb r5, 0x1b8 + 0x80 (r30)

    lwz r0, 0x0 (r3) // Original instruction
    b DemoPage_onInit + 0x130
PATCH_B_END(DemoPage_onInit, 0x12c)



PATCH_BL_START(DemoPage_onInit, 0x1ec)
    addi r0, r3, 0x589 // Original instruction

    // Check that 200cc is enabled
    lis r5, vsSpeedModIsEnabled@ha
    lbz r5, vsSpeedModIsEnabled@l (r5)
    cmpwi r5, 0x0
    beqlr

    // Set the message id for 200cc
    li r0, 10072
    b DemoPage_onInit + 0x1f0
PATCH_BL_END(DemoPage_onInit, 0x1ec)
