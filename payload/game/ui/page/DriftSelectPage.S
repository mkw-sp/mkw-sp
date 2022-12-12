#include "../SectionId.S"
#include <Common.S>

// Initialize the previous page id for both "Change Course" and "Change Ghost Data"

PATCH_BL_START(DriftSelectPage_onInit, 0x98)
    cmpwi r0, 0x4a // Original instruction
    beqlr

    cmpwi r0, SingleChangeGhostData
    b DriftSelectPage_onInit + 0x9c
PATCH_BL_END(DriftSelectPage_onInit, 0x98)
