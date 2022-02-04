#include <Common.S>

PATCH_BL_START(DemoPage_onInit, 0x1ec)
    // Adjust engine classes to handle the removal of 50cc and the addition of 200cc
    addi r0, r3, 0x588

    // Check that 200cc is enabled
    lis r5, vsSpeedModIsEnabled@ha
    lbz r5, vsSpeedModIsEnabled@l (r5)
    cmpwi r5, 0x0
    beqlr

    // Set the message id for 200cc
    li r0, 0x58b
    b DemoPage_onInit + 0x1f0
PATCH_BL_END(DemoPage_onInit, 0x1ec)
