#include <Common.S>

// Initialize the previous page id for both "Change Course" and "Change Ghost Data"

PATCH_BL_START(MachineSelectPage_onInit, 0x48)
    cmpwi r0, 0x4a // Original instruction
    beqlr

    cmpwi r0, 0x4d
    b MachineSelectPage_onInit + 0x4c
PATCH_BL_END(MachineSelectPage_onInit, 0x48)
