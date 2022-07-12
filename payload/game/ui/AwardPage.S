#include <Common.S>

PATCH_BL_START(AwardPage_initType, 0xc4)
    li r0, 1419 // Original instruction

    // Check that 200cc is enabled
    lis r3, vsSpeedModIsEnabled@ha
    lbz r3, vsSpeedModIsEnabled@l (r3)
    cmpwi r3, 0x0
    beqlr

    // Set the message id for 200cc
    li r0, 10072
    b AwardPage_initType + 0xc8
PATCH_BL_END(AwardPage_initType, 0xc4)

PATCH_BL_START(AwardPage_initType, 0x13c)
    li r0, 1419 // Original instruction

    // Check that 200cc is enabled
    lis r3, vsSpeedModIsEnabled@ha
    lbz r3, vsSpeedModIsEnabled@l (r3)
    cmpwi r3, 0x0
    beqlr

    // Set the message id for 200cc
    li r0, 10072
    b AwardPage_initType + 0x140
PATCH_BL_END(AwardPage_initType, 0x13c)
