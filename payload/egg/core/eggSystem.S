#include <Common.S>

// Reduce the size of the system heap to 640 KiB

#define SYSTEM_HEAP_SIZE 0xa0000

PATCH_B_START(EGG_ConfigurationData_initMemory, 0x158)
    lis r3, SYSTEM_HEAP_SIZE@h
    ori r3, r3, SYSTEM_HEAP_SIZE@l
    b EGG_ConfigurationData_initMemory + 0x15c
PATCH_B_END(EGG_ConfigurationData_initMemory, 0x158)
